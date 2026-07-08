// Fill out your copyright notice in the Description page of Project Settings.

#include "AdvancedVehicleManagerAsyncCallback.h"
#include "AdvancedVehiclePawn.h"
#include "Chaos/ParticleHandleFwd.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "PBDRigidsSolver.h"

DECLARE_CYCLE_STAT(TEXT("AdvancedVehicleManagerAsync:ProcessInputs"), STAT_AdvancedManagerAsync_OnProcessInputs, STATGROUP_AdvancedVehicleManagerAsync);
DECLARE_CYCLE_STAT(TEXT("AdvancedVehicleManagerAsync:PreSimulate"), STAT_AdvancedManagerAsync_OnPreSimulate, STATGROUP_AdvancedVehicleManagerAsync);

DECLARE_CYCLE_STAT(TEXT("AdvancedVehicleManagerAsync:ChaosSingleThread"), STAT_AdvancedManagerAsync_ChaosSingleThread, STATGROUP_AdvancedVehicleManagerAsync);
DECLARE_CYCLE_STAT(TEXT("AdvancedVehicleManagerAsync:ChaosParallelFor"), STAT_AdvancedManagerAsync_ChaosParallelFor, STATGROUP_AdvancedVehicleManagerAsync);

static bool bAdvancedVehicleEnableSimulation = true;
static bool bAdvancedVehicleSingleThread = true;

FAutoConsoleVariableRef CVarAdvancedVehicleManagerPhysicsUpdate(TEXT("p.AdvancedVehicle.PhysicsUpdate"), bAdvancedVehicleEnableSimulation, TEXT("Enable/Disable physics simulation update. Userful for debugging"));
FAutoConsoleVariableRef CVarAdvancedVehicleManagerSingleThread(TEXT("p.AdvancedVehicle.SingleThread"), bAdvancedVehicleSingleThread, TEXT("Enable/Disable physics single or multithread (ChaosParallelFor) simulation update."));

void FAdvancedVehicleManagerAsyncInput::ApplySimulate(UWorld* InWorld, const float InDeltaTime, const float InSimTime) const
{
	if (IsValid(Vehicle) && Proxy)
	{
		Vehicle->PhysicsTick(InWorld, InDeltaTime, InSimTime, Proxy->GetPhysicsThreadAPI());
	}
}

void FAdvancedVehicleManagerAsyncInput::ApplyDeferredForces(Chaos::FRigidBodyHandle_Internal* InRigidHandle) const
{
	if (IsValid(Vehicle) && InRigidHandle)
	{
		Vehicle->ApplyVehicleForces(InRigidHandle);
	}
}

FName FAdvancedVehicleManagerAsyncCallback::GetFNameForStatId() const
{
	const static FLazyName StaticName("FAdvancedVehicleManagerAsyncCallback");
	return StaticName;
}

void FAdvancedVehicleManagerAsyncCallback::ProcessInputs_Internal(int32 PhysicsStep)
{
	SCOPE_CYCLE_COUNTER(STAT_AdvancedManagerAsync_OnProcessInputs);
}

void FAdvancedVehicleManagerAsyncCallback::OnPreSimulate_Internal()
{
	SCOPE_CYCLE_COUNTER(STAT_AdvancedManagerAsync_OnPreSimulate);

	const Chaos::FReal DeltaTime = GetDeltaTime_Internal();
	const Chaos::FReal SimTime = GetSimTime_Internal();

	const FAdvancedVehicleManagerAsyncCallbackInput* AsyncInput = GetConsumerInput_Internal();
	if (!AsyncInput) return;

	UWorld* World = AsyncInput->World.Get();
	if (!World) return;

	const int32 NumVehicles = AsyncInput->VehicleInputs.Num();
	if (NumVehicles == 0) return;
	
	//const Chaos::FPhysicsSolverBase* ChaosSolverBase = GetSolver();
	//if (!ChaosSolverBase) return;

	FAdvancedVehicleManagerAsyncCallbackOutput& AsyncOutput = GetProducerOutputData_Internal();
	AsyncOutput.Reset();
	AsyncOutput.Timestamp = AsyncInput->Timestamp;
	AsyncOutput.VehicleOutputs.SetNum(NumVehicles);

	if (!bAdvancedVehicleSingleThread) return;
	const TArray<TUniquePtr<FAdvancedVehicleManagerAsyncInput>>& InputVehicleBatch = AsyncInput->VehicleInputs;

	auto SimulateVehicle = [World, DeltaTime, SimTime, &InputVehicleBatch](const int32 idx)
	{
		auto& VehicleInput = InputVehicleBatch[idx];
		if (!VehicleInput.IsValid()) return;
		
		if (const Chaos::FSingleParticlePhysicsProxy* Proxy = VehicleInput->Proxy)
		{
			if (const Chaos::FRigidBodyHandle_Internal* VehicleHandle = Proxy->GetPhysicsThreadAPI())
			{
				if (VehicleHandle->ObjectState() != Chaos::EObjectStateType::Dynamic)
				{
					return;
				}

				VehicleInput->ApplySimulate(World, DeltaTime, SimTime);
			}
		}
	};

	if (bAdvancedVehicleSingleThread)
	{
		SCOPE_CYCLE_COUNTER(STAT_AdvancedManagerAsync_ChaosSingleThread);
		{
			for (int32 i = 0; i < NumVehicles; ++i)
			{
				SimulateVehicle(i);
			}
		}
	}
	else
	{
		SCOPE_CYCLE_COUNTER(STAT_AdvancedManagerAsync_ChaosParallelFor);
		{
			Chaos::PhysicsParallelFor(NumVehicles, SimulateVehicle, false);
		}
	}

	for (const TUniquePtr<FAdvancedVehicleManagerAsyncInput>& VehicleInput : InputVehicleBatch)
	{
		if (Chaos::FSingleParticlePhysicsProxy* const VehicleProxy = VehicleInput->Proxy)
		{
			if (Chaos::FRigidBodyHandle_Internal* const VehicleHandle = VehicleProxy->GetPhysicsThreadAPI())
			{
				VehicleInput->ApplyDeferredForces(VehicleHandle);
			}
		}
	}
}