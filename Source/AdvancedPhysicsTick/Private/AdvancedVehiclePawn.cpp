// Copyright Alexander. All Rights Reserved.

#include "AdvancedVehiclePawn.h"
#include "ManagerAsyncCallback.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "Chaos/Particle/ParticleUtilities.h"
#include "Chaos/Utilities.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "DrawDebugHelpers.h"
#include "DisplayDebugHelpers.h"

AAdvancedVehiclePawn::AAdvancedVehiclePawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AsyncCallback = nullptr;
}

void AAdvancedVehiclePawn::BeginPlay()
{
	Super::BeginPlay();

	AsyncCallback = GetWorld()->GetPhysicsScene()->GetSolver()->CreateAndRegisterSimCallbackObject_External<FManagerAsyncCallback>();
	AsyncCallback->InitAsyncCallback(this);
}

void AAdvancedVehiclePawn::TickAsync(float DeltaTime, float SimTime)
{
	AdvancedTick(DeltaTime, SimTime);
}

void AAdvancedVehiclePawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (AsyncCallback)
	{
		GetWorld()->GetPhysicsScene()->GetSolver()->UnregisterAndFreeSimCallbackObject_External(AsyncCallback);
		AsyncCallback = nullptr;
	}
}

void AAdvancedVehiclePawn::AddForce(const UPrimitiveComponent* InComponent, FVector Force, bool bAccelChange)
{
	if (FPhysicsActorHandle ActorHandle = InComponent->GetBodyInstance()->ActorHandle)
	{
		if (IsInGameThread()) return;
		if (Chaos::FRigidBodyHandle_Internal* RigidHandle = ActorHandle->GetPhysicsThreadAPI())
		{
			if (ensure(RigidHandle))
			{
				if (bAccelChange)
				{
					const float RigidMass = RigidHandle->M();
					const Chaos::FVec3 Acceleration = Force * RigidMass;
					RigidHandle->AddForce(Acceleration, false);
				}
				else RigidHandle->AddForce(Force, false);
			}
		}
	}
}

void AAdvancedVehiclePawn::AddForceAtLocation(const UPrimitiveComponent* InComponent, FVector Force, FVector Position, bool bIsLocalForce)
{
	if (FPhysicsActorHandle ActorHandle = InComponent->GetBodyInstance()->ActorHandle)
	{
		if (IsInGameThread()) return;
		if (Chaos::FRigidBodyHandle_Internal* RigidHandle = ActorHandle->GetPhysicsThreadAPI())
		{
			if (ensure(RigidHandle))
			{
				const Chaos::FVec3 WorldCOM = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(RigidHandle);
				const Chaos::FVec3 WorldTorque = Chaos::FVec3::CrossProduct(Position - WorldCOM, Force);
				RigidHandle->AddForce(Force, false);
				RigidHandle->AddTorque(WorldTorque, false);
			}
		}
	}
}

void AAdvancedVehiclePawn::AddTorque(const UPrimitiveComponent* InComponent, FVector Torque, bool bAccelChange)
{
	if (FPhysicsActorHandle ActorHandle = InComponent->GetBodyInstance()->ActorHandle)
	{
		if (IsInGameThread()) return;
		if (Chaos::FRigidBodyHandle_Internal* RigidHandle = ActorHandle->GetPhysicsThreadAPI())
		{
			if (ensure(RigidHandle))
			{
				if (bAccelChange) RigidHandle->AddTorque(Chaos::FParticleUtilitiesXR::GetWorldInertia(RigidHandle) * Torque, false);
				else RigidHandle->AddTorque(Torque, false);
			}
		}
	}
}

void AAdvancedVehiclePawn::AddImpulse(const UPrimitiveComponent* InComponent, FVector Impulse, bool bVelChange)
{
	if (FPhysicsActorHandle ActorHandle = InComponent->GetBodyInstance()->ActorHandle)
	{
		if (IsInGameThread()) return;
		if (Chaos::FRigidBodyHandle_Internal* RigidHandle = ActorHandle->GetPhysicsThreadAPI())
		{
			if (ensure(RigidHandle))
			{
				if (bVelChange) RigidHandle->SetLinearImpulse(RigidHandle->LinearImpulse() + RigidHandle->M() * Impulse, false);
				else RigidHandle->SetLinearImpulse(RigidHandle->LinearImpulse() + Impulse, false);
			}
		}
	}
}

void AAdvancedVehiclePawn::AddImpulseAtLocation(const UPrimitiveComponent* InComponent, FVector Impulse, FVector Position)
{
	if (FPhysicsActorHandle ActorHandle = InComponent->GetBodyInstance()->ActorHandle)
	{
		if (IsInGameThread()) return;
		if (Chaos::FRigidBodyHandle_Internal* RigidHandle = ActorHandle->GetPhysicsThreadAPI())
		{
			if (ensure(RigidHandle))
			{
				const Chaos::FVec3 WorldCOM = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(RigidHandle);
				const Chaos::FVec3 AngularImpulse = Chaos::FVec3::CrossProduct(Position - WorldCOM, Impulse);
				RigidHandle->SetLinearImpulse(RigidHandle->LinearImpulse() + Impulse, false);
				RigidHandle->SetAngularImpulse(RigidHandle->AngularImpulse() + AngularImpulse, false);
			}
		}
	}
}

FTransform AAdvancedVehiclePawn::GetPrimitiveWorldTransform(const UPrimitiveComponent* InComponent) const
{
	if (FPhysicsActorHandle ActorHandle = InComponent->GetBodyInstance()->ActorHandle)
	{
		if (!IsInGameThread())
		{
			if (Chaos::FRigidBodyHandle_Internal* RigidHandle = ActorHandle->GetPhysicsThreadAPI())
			{
				if (ensure(RigidHandle)) return FTransform(RigidHandle->R(), RigidHandle->X());
			}
		}
		else
		{
			Chaos::FRigidBodyHandle_External& RigidHandle = ActorHandle->GetGameThreadAPI();
			return FTransform(RigidHandle.R(), RigidHandle.X());
		}
	}
	return FTransform();
}

FVector AAdvancedVehiclePawn::GetLinearVelocity(const UPrimitiveComponent* InComponent)
{
	if (FPhysicsActorHandle ActorHandle = InComponent->GetBodyInstance()->ActorHandle)
	{
		if (!IsInGameThread())
		{
			if (Chaos::FRigidBodyHandle_Internal* RigidHandle = ActorHandle->GetPhysicsThreadAPI())
			{
				if (ensure(RigidHandle)) return RigidHandle->V();
			}
		}
		else
		{
			Chaos::FRigidBodyHandle_External& RigidHandle = ActorHandle->GetGameThreadAPI();
			return RigidHandle.V();
		}
	}
	return FVector::ZeroVector;
}

FVector AAdvancedVehiclePawn::GetLinearVelocityAtPoint(const UPrimitiveComponent* InComponent, FVector Point)
{
	if (FPhysicsActorHandle ActorHandle = InComponent->GetBodyInstance()->ActorHandle)
	{
		if (!IsInGameThread())
		{
			if (Chaos::FRigidBodyHandle_Internal* RigidHandle = ActorHandle->GetPhysicsThreadAPI())
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
			Chaos::FRigidBodyHandle_External& RigidHandle = ActorHandle->GetGameThreadAPI();
			const Chaos::FVec3 COM = RigidHandle.X() + RigidHandle.R() * RigidHandle.CenterOfMass();
			const Chaos::FVec3 Diff = Point - COM;
			return RigidHandle.V() - Chaos::FVec3::CrossProduct(Diff, RigidHandle.W());
		}
	}
	return FVector::ZeroVector;
}