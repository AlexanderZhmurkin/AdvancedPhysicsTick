// Fill out your copyright notice in the Description page of Project Settings.

#include "AdvancedVehicleManager.h"
#include "AdvancedVehicleMovement.h"
#include "AdvancedVehiclePawn.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "Physics/PhysicsFiltering.h"
#include "Physics/PhysicsInterfaceCore.h"
#include "PBDRigidsSolver.h"

DECLARE_CYCLE_STAT(TEXT("AdvancedVehicle:ScenePreTick"), STAT_AdvancedVehicle_ScenePreTick, STATGROUP_AdvancedVehicle);
DECLARE_CYCLE_STAT(TEXT("AdvancedVehicle:ScenePostTick"), STAT_AdvancedVehicle_ScenePostTick, STATGROUP_AdvancedVehicle);

DECLARE_DWORD_ACCUMULATOR_STAT(TEXT("AdvancedVehicle:Timestamp"), STAT_AdvancedVehicle_Timestamp, STATGROUP_AdvancedVehicle);
DECLARE_DWORD_ACCUMULATOR_STAT(TEXT("AdvancedVehicle:TotalCount"), STAT_AdvancedVehicle_TotalCount, STATGROUP_AdvancedVehicle);

bool FAdvancedVehicleManager::bInitialized = false;
TMap<FPhysScene*, FAdvancedVehicleManager*> FAdvancedVehicleManager::SceneToPhysicsManagerMap;

FDelegateHandle FAdvancedVehicleManager::OnPostWorldInitHandle;
FDelegateHandle FAdvancedVehicleManager::OnWorldCleanupHandle;

FAdvancedVehicleManager::FAdvancedVehicleManager(FPhysScene* InPhysScene)
	: Scene(*InPhysScene)
	, AsyncCallback(nullptr)
{
	check(InPhysScene);

	if (!bInitialized)
	{
		OnPostWorldInitHandle = FWorldDelegates::OnPostWorldInitialization.AddStatic(&FAdvancedVehicleManager::OnPostWorldInitialization);
		OnWorldCleanupHandle = FWorldDelegates::OnWorldCleanup.AddStatic(&FAdvancedVehicleManager::OnWorldCleanup);

		bInitialized = true;
	}

	ensure(FAdvancedVehicleManager::SceneToPhysicsManagerMap.Find(InPhysScene) == nullptr);
	FAdvancedVehicleManager::SceneToPhysicsManagerMap.Add(InPhysScene, this);
}

FAdvancedVehicleManager::~FAdvancedVehicleManager()
{
	while (Vehicles.Num() > 0)
	{
		RemoveVehicle(Vehicles.Last());
	}

	UnregisterCallbacks();
	DetachManagerFromPhysScene(&Scene);
}

void FAdvancedVehicleManager::OnPostWorldInitialization(UWorld* InWorld, const UWorld::InitializationValues)
{
	if (FAdvancedVehicleManager* const VehicleManager = FAdvancedVehicleManager::GetManagerFromScene(InWorld->GetPhysicsScene()))
	{
		VehicleManager->RegisterCallbacks();
	}
}

void FAdvancedVehicleManager::OnWorldCleanup(UWorld* InWorld, bool bSessionEnded, bool bCleanupResources)
{
	if (FAdvancedVehicleManager* const VehicleManager = FAdvancedVehicleManager::GetManagerFromScene(InWorld->GetPhysicsScene()))
	{
		VehicleManager->UnregisterCallbacks();
	}
}

void FAdvancedVehicleManager::RegisterCallbacks()
{
	OnPhysScenePreTickHandle = Scene.OnPhysScenePreTick.AddRaw(this, &FAdvancedVehicleManager::PreUpdate);
	OnPhysScenePostTickHandle = Scene.OnPhysScenePostTick.AddRaw(this, &FAdvancedVehicleManager::PostUpdate);

	if (!AsyncCallback)
	{
		AsyncCallback = Scene.GetSolver()->CreateAndRegisterSimCallbackObject_External<FAdvancedVehicleManagerAsyncCallback>();
	}
}

void FAdvancedVehicleManager::UnregisterCallbacks()
{
	if (OnPhysScenePreTickHandle.IsValid())
	{
		Scene.OnPhysScenePreTick.Remove(OnPhysScenePreTickHandle);
		OnPhysScenePreTickHandle.Reset();
	}

	if (OnPhysScenePostTickHandle.IsValid())
	{
		Scene.OnPhysScenePostTick.Remove(OnPhysScenePostTickHandle);
		OnPhysScenePostTickHandle.Reset();
	}

	if (AsyncCallback)
	{
		if (Chaos::FPhysicsSolver* Solver = Scene.GetSolver())
		{
			Solver->UnregisterAndFreeSimCallbackObject_External(AsyncCallback);
		}

		AsyncCallback = nullptr;
	}
}

void FAdvancedVehicleManager::PreUpdate(FPhysScene* InPhysScene, float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_AdvancedVehicle_ScenePreTick);

	UWorld* World = Scene.GetOwningWorld();
	if (!World) return;

	//for (int32 i = 0; i < Vehicles.Num(); ++i)
	//{
	//	Vehicles[i]->PreTick(DeltaTime);
	//}

	FAdvancedVehicleManagerAsyncCallbackInput* AsyncInput = AsyncCallback->GetProducerInputData_External();
	AsyncInput->Reset();
	AsyncInput->VehicleInputs.Reserve(Vehicles.Num());
	AsyncInput->Timestamp = Timestamp;
	AsyncInput->World = World;

	for (TWeakObjectPtr<AAdvancedVehiclePawn> Vehicle : Vehicles)
	{
		if (!Vehicle.IsValid()) continue;
		const int32 InputIdx = AsyncInput->VehicleInputs.Num();
		AsyncInput->VehicleInputs.Add(Vehicle->BuildAsyncInput(InputIdx, Timestamp));
	}

	++Timestamp;
}

void FAdvancedVehicleManager::PostUpdate(FChaosScene* InPhysScene)
{
	SCOPE_CYCLE_COUNTER(STAT_AdvancedVehicle_ScenePostTick);
	SET_DWORD_STAT(STAT_AdvancedVehicle_TotalCount, Vehicles.Num());
}

void FAdvancedVehicleManager::AddVehicle(TWeakObjectPtr<AAdvancedVehiclePawn> InVehicle)
{
	check(InVehicle != NULL);
	check(AsyncCallback);

	if (InVehicle.IsValid())
	{
		Vehicles.Add(InVehicle);
	}
}

void FAdvancedVehicleManager::RemoveVehicle(TWeakObjectPtr<AAdvancedVehiclePawn> InVehicle)
{
	check(InVehicle != NULL);

	if (InVehicle.IsValid())
	{
		Vehicles.Remove(InVehicle);
	}
}

FAdvancedVehicleManager* FAdvancedVehicleManager::GetManagerFromScene(FPhysScene* InPhysScene)
{
	return SceneToPhysicsManagerMap.FindRef(InPhysScene);
}

void FAdvancedVehicleManager::DetachManagerFromPhysScene(FPhysScene* InPhysScene)
{
	FAdvancedVehicleManager::SceneToPhysicsManagerMap.Remove(InPhysScene);
}