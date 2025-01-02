// Copyright Alexander. All Rights Reserved.

#include "AdvancedPawn.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "Chaos/Particle/ParticleUtilities.h"
#include "Chaos/Utilities.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "Chaos/DebugDrawQueue.h"
#include "DrawDebugHelpers.h"
#include "DisplayDebugHelpers.h"

DECLARE_CYCLE_STAT(TEXT("AdvancedPawn:TickAsync"), STAT_AdvancedManager_TickAsync, STATGROUP_AdvancedManager);

static float DrawDebugLinesSize = 0.0005f;
static bool DrawDebugTextForces = false;
static bool DrawDebugAllForces = false;

FAutoConsoleVariableRef CVarDebugAllForceIndex(TEXT("p.AdvancedPawn.DrawDebugAllForces"), DrawDebugAllForces, TEXT("Toggle drawing 3D arrows representing forces applied by AddForce."));
FAutoConsoleVariableRef CVarDebugTextForcesIndex(TEXT("p.AdvancedPawn.DrawDebugTextForces"), DrawDebugTextForces, TEXT("Toggle drawing 3D text representing forces applied by AddForce."));
FAutoConsoleVariableRef CVarDebugLinesSizeIndex(TEXT("p.AdvancedPawn.DrawDebugLinesSize"), DrawDebugLinesSize, TEXT("Set the size/length multiplier for 3D debug arrows drawn by AddForce."));

AAdvancedPawn::AAdvancedPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), World(nullptr), AsyncCallback(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAdvancedPawn::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	if (!World || !World->IsGameWorld()) return;

	FPhysScene* PhysScene = World->GetPhysicsScene();
	if (!PhysScene || !PhysScene->GetSolver()) return;
	AsyncCallback = PhysScene->GetSolver()->CreateAndRegisterSimCallbackObject_External<FAdvancedManagerAsyncCallback>();
	AsyncCallback->InitAsyncCallback(this);
}

void AAdvancedPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (!AsyncCallback) return;
	FPhysScene* PhysScene = GetWorld()->GetPhysicsScene();
	PhysScene->GetSolver()->UnregisterAndFreeSimCallbackObject_External(AsyncCallback);
	AsyncCallback = nullptr;
}

void AAdvancedPawn::TickAsync(float DeltaTime, float SimTime)
{
	SCOPE_CYCLE_COUNTER(STAT_AdvancedManager_TickAsync);

	// Called this function in FAdvancedManagerAsyncCallback::OnPreSimulate_Internal
	AdvancedTick(DeltaTime, SimTime);
}

void AAdvancedPawn::AddForce(const UPrimitiveComponent* InComponent, FVector Force, bool bAccelChange)
{
	if (!InComponent) return;
	const auto Handle = InComponent->GetBodyInstance()->ActorHandle;
	if (!Handle) return;

	if (!IsInGameThread())
	{
		Chaos::FRigidBodyHandle_Internal* RigidHandle = Handle->GetPhysicsThreadAPI();
		if (!ensure(RigidHandle)) return;

		if (bAccelChange)
		{
			const float RigidMass = RigidHandle->M();
			const Chaos::FVec3 Acceleration = Force * RigidMass;
			RigidHandle->AddForce(Acceleration, false);
		}
		else RigidHandle->AddForce(Force, false);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		if (DrawDebugAllForces)
		{
			FVector Position = RigidHandle->X();
			::DrawDebugDirectionalArrow(GetWorld(), Position, Position + Force * DrawDebugLinesSize, 20.0f, FColor::White, false, 0, SDPG_World, 2.0f);

			if (DrawDebugTextForces)
			{
				const FString ForceLabel = FString::Printf(TEXT("Force: %.2f"), Force.Size());
				::DrawDebugString(GetWorld(), Position + FVector(0, 0, 50), ForceLabel, nullptr, FColor::White, 0, true);
			}
		}
#endif
	}
}

void AAdvancedPawn::AddForceAtLocation(const UPrimitiveComponent* InComponent, FVector Force, FVector Position, bool bIsLocalForce)
{
	if (!InComponent) return;
	const auto Handle = InComponent->GetBodyInstance()->ActorHandle;
	if (!Handle) return;

	if (!IsInGameThread())
	{
		Chaos::FRigidBodyHandle_Internal* RigidHandle = Handle->GetPhysicsThreadAPI();
		if (!ensure(RigidHandle)) return;

		const Chaos::FVec3 WorldCOM = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(RigidHandle);
		const Chaos::FVec3 WorldTorque = Chaos::FVec3::CrossProduct(Position - WorldCOM, Force);
		RigidHandle->AddForce(Force, false);
		RigidHandle->AddTorque(WorldTorque, false);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		if (DrawDebugAllForces)
		{
			::DrawDebugDirectionalArrow(GetWorld(), Position, Position + Force * DrawDebugLinesSize, 20.0f, FColor::White, false, 0, SDPG_World, 2.0f);

			if (DrawDebugTextForces)
			{
				const FString ForceLabel = FString::Printf(TEXT("Force: %.2f"), Force.Size());
				::DrawDebugString(GetWorld(), Position + FVector(0, 0, 50), ForceLabel, nullptr, FColor::White, 0, true);
			}
		}
#endif
	}
}

void AAdvancedPawn::AddTorque(const UPrimitiveComponent* InComponent, FVector Torque, bool bAccelChange)
{
	if (!InComponent) return;
	const auto Handle = InComponent->GetBodyInstance()->ActorHandle;
	if (!Handle) return;

	if (!IsInGameThread())
	{
		Chaos::FRigidBodyHandle_Internal* RigidHandle = Handle->GetPhysicsThreadAPI();
		if (!ensure(RigidHandle)) return;

		if (bAccelChange) RigidHandle->AddTorque(Chaos::FParticleUtilitiesXR::GetWorldInertia(RigidHandle) * Torque, false);
		else RigidHandle->AddTorque(Torque, false);
	}
}

void AAdvancedPawn::AddImpulse(const UPrimitiveComponent* InComponent, FVector Impulse, bool bVelChange)
{
	if (!InComponent) return;
	const auto Handle = InComponent->GetBodyInstance()->ActorHandle;
	if (!Handle) return;

	if (!IsInGameThread())
	{
		Chaos::FRigidBodyHandle_Internal* RigidHandle = Handle->GetPhysicsThreadAPI();
		if (!ensure(RigidHandle)) return;

		if (bVelChange) RigidHandle->SetLinearImpulse(RigidHandle->LinearImpulse() + RigidHandle->M() * Impulse, false);
		else RigidHandle->SetLinearImpulse(RigidHandle->LinearImpulse() + Impulse, false);
	}
}

void AAdvancedPawn::AddImpulseAtLocation(const UPrimitiveComponent* InComponent, FVector Impulse, FVector Position)
{
	if (!InComponent) return;
	const auto Handle = InComponent->GetBodyInstance()->ActorHandle;
	if (!Handle) return;

	if (!IsInGameThread())
	{
		Chaos::FRigidBodyHandle_Internal* RigidHandle = Handle->GetPhysicsThreadAPI();
		if (!ensure(RigidHandle)) return;

		const Chaos::FVec3 WorldCOM = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(RigidHandle);
		const Chaos::FVec3 AngularImpulse = Chaos::FVec3::CrossProduct(Position - WorldCOM, Impulse);
		RigidHandle->SetLinearImpulse(RigidHandle->LinearImpulse() + Impulse, false);
		RigidHandle->SetAngularImpulse(RigidHandle->AngularImpulse() + AngularImpulse, false);
	}
}

FTransform AAdvancedPawn::GetPrimitiveWorldTransform(const UPrimitiveComponent* InComponent) const
{
	if (InComponent)
	{
		const auto Handle = InComponent->GetBodyInstance()->ActorHandle;
		if (Handle)
		{
			if (!IsInGameThread())
			{
				Chaos::FRigidBodyHandle_Internal* RigidHandle = Handle->GetPhysicsThreadAPI();
				if (ensure(RigidHandle)) return FTransform(RigidHandle->R(), RigidHandle->X());
			}
			else
			{
				Chaos::FRigidBodyHandle_External& RigidHandle = Handle->GetGameThreadAPI();
				return FTransform(RigidHandle.R(), RigidHandle.X());
			}
		}
	}

	return FTransform();
}

FVector AAdvancedPawn::GetLinearVelocity(const UPrimitiveComponent* InComponent)
{
	if (InComponent)
	{
		const auto Handle = InComponent->GetBodyInstance()->ActorHandle;
		if (Handle)
		{
			if (!IsInGameThread())
			{
				Chaos::FRigidBodyHandle_Internal* RigidHandle = Handle->GetPhysicsThreadAPI();
				if (ensure(RigidHandle)) return RigidHandle->V();
			}
			else
			{
				Chaos::FRigidBodyHandle_External& RigidHandle = Handle->GetGameThreadAPI();
				return RigidHandle.V();
			}
		}
	}

	return FVector::ZeroVector;
}

FVector AAdvancedPawn::GetLinearVelocityAtPoint(const UPrimitiveComponent* InComponent, FVector Point)
{
	if (InComponent)
	{
		const auto Handle = InComponent->GetBodyInstance()->ActorHandle;
		if (Handle)
		{
			if (!IsInGameThread())
			{
				if (Chaos::FRigidBodyHandle_Internal* RigidHandle = Handle->GetPhysicsThreadAPI())
				{
					if (ensure(RigidHandle))
					{
						const Chaos::FVec3 COM = RigidHandle->X() + RigidHandle->R() * RigidHandle->CenterOfMass();
						const Chaos::FVec3 Diff = Point - COM;
						return RigidHandle->V() - Chaos::FVec3::CrossProduct(Diff, RigidHandle->W());
					}
				}
			}
			else
			{
				Chaos::FRigidBodyHandle_External& RigidHandle = Handle->GetGameThreadAPI();
				const Chaos::FVec3 COM = RigidHandle.X() + RigidHandle.R() * RigidHandle.CenterOfMass();
				const Chaos::FVec3 Diff = Point - COM;
				return RigidHandle.V() - Chaos::FVec3::CrossProduct(Diff, RigidHandle.W());
			}
		}
	}

	return FVector::ZeroVector;
}

FVector AAdvancedPawn::GetPrimitiveCOM(const UPrimitiveComponent* InComponent) const
{
	if (InComponent)
	{
		const auto Handle = InComponent->GetBodyInstance()->ActorHandle;
		if (Handle)
		{
			if (!IsInGameThread())
			{
				if (Chaos::FRigidBodyHandle_Internal* RigidHandle = Handle->GetPhysicsThreadAPI())
				{
					if (ensure(RigidHandle)) return RigidHandle->X() + RigidHandle->R() * RigidHandle->CenterOfMass();
				}
			}
			else
			{
				Chaos::FRigidBodyHandle_External& RigidHandle = Handle->GetGameThreadAPI();
				return RigidHandle.X() + RigidHandle.R() * RigidHandle.CenterOfMass();
			}
		}
	}

	return FVector::ZeroVector;
}