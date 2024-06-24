// Copyright Alexander. All Rights Reserved.

#include "NWheelVehicle.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "Chaos/Particle/ParticleUtilities.h"
#include "Chaos/Utilities.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "DrawDebugHelpers.h"
#include "DisplayDebugHelpers.h"

void FExtendedAsyncPawn::Initialize(ANWheelVehicle* InVehicle)
{
	Vehicle = InVehicle;
}

void FExtendedAsyncPawn::OnPreSimulate_Internal()
{
	check(Vehicle);

	float DeltaTime = GetDeltaTime_Internal();
	float SimTime = GetSimTime_Internal();

	Vehicle->TickAsync(DeltaTime, SimTime);
}

ANWheelVehicle::ANWheelVehicle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AsyncCallback = nullptr;
}

void ANWheelVehicle::BeginPlay()
{
	Super::BeginPlay();

	AsyncCallback = GetWorld()->GetPhysicsScene()->GetSolver()->CreateAndRegisterSimCallbackObject_External<FExtendedAsyncPawn>();
	AsyncCallback->Initialize(this);
}

void ANWheelVehicle::TickAsync(float DeltaTime, float SimTime)
{
	AsyncTick(DeltaTime, SimTime);
}

void ANWheelVehicle::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AsyncCallback)
	{
		GetWorld()->GetPhysicsScene()->GetSolver()->UnregisterAndFreeSimCallbackObject_External(AsyncCallback);
		AsyncCallback = nullptr;
	}
}

void ANWheelVehicle::AddForce(UPrimitiveComponent* Component, FVector Force, bool bAccelChange)
{
	if (Chaos::FRigidBodyHandle_Internal* RigidHandle = GetInternalHandle(Component))
	{
		if (bAccelChange)
		{
			const float RigidMass = RigidHandle->M();
			const Chaos::FVec3 Acceleration = Force * RigidMass;
			RigidHandle->AddForce(Acceleration, false);
		}
		else
		{
			RigidHandle->AddForce(Force, false);
		}
	}
}

void ANWheelVehicle::AddForceAtPosition(UPrimitiveComponent* Component, FVector Force, FVector Position, bool bIsLocalForce)
{
	if (Chaos::FRigidBodyHandle_Internal* RigidHandle = GetInternalHandle(Component))
	{
		const Chaos::FVec3 WorldCOM = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(RigidHandle);
		const Chaos::FVec3 WorldTorque = Chaos::FVec3::CrossProduct(Position - WorldCOM, Force);
		RigidHandle->AddForce(Force, false);
		RigidHandle->AddTorque(WorldTorque, false);
	}
}

void ANWheelVehicle::AddTorque(UPrimitiveComponent* Component, FVector Torque, bool bAccelChange)
{
	if (Chaos::FRigidBodyHandle_Internal* RigidHandle = GetInternalHandle(Component))
	{
		if (bAccelChange) RigidHandle->AddTorque(Chaos::FParticleUtilitiesXR::GetWorldInertia(RigidHandle) * Torque, false);
		else RigidHandle->AddTorque(Torque, false);
	}
}

void ANWheelVehicle::AddImpulse(UPrimitiveComponent* Component, FVector Impulse, bool bVelChange)
{
	if (Chaos::FRigidBodyHandle_Internal* RigidHandle = GetInternalHandle(Component))
	{
		if (bVelChange) RigidHandle->SetLinearImpulse(RigidHandle->LinearImpulse() + RigidHandle->M() * Impulse, false);
		else RigidHandle->SetLinearImpulse(RigidHandle->LinearImpulse() + Impulse, false);
	}
}

void ANWheelVehicle::AddImpulseAtPosition(UPrimitiveComponent* Component, FVector Impulse, const FVector Position)
{
	if (Chaos::FRigidBodyHandle_Internal* RigidHandle = GetInternalHandle(Component))
	{
		const Chaos::FVec3 WorldCOM = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(RigidHandle);
		const Chaos::FVec3 AngularImpulse = Chaos::FVec3::CrossProduct(Position - WorldCOM, Impulse);
		RigidHandle->SetLinearImpulse(RigidHandle->LinearImpulse() + Impulse, false);
		RigidHandle->SetAngularImpulse(RigidHandle->AngularImpulse() + AngularImpulse, false);
	}
}

void ANWheelVehicle::AddAngularImpulseInRadians(UPrimitiveComponent* Component, FVector Impulse, bool bAccelChange)
{
	if (Chaos::FRigidBodyHandle_Internal* RigidHandle = GetInternalHandle(Component))
	{
		if (bAccelChange)
		{
			const Chaos::FMatrix33 WorldI = Chaos::FParticleUtilitiesXR::GetWorldInertia(RigidHandle);
			RigidHandle->SetAngularImpulse(RigidHandle->AngularImpulse() + (WorldI * Impulse), false);
		}
		else
		{
			RigidHandle->SetAngularImpulse(RigidHandle->AngularImpulse() + Impulse, false);
		}
	}
}

void ANWheelVehicle::AddAngularImpulseInDegrees(UPrimitiveComponent* Component, FVector Impulse, bool bAccelChange)
{
	AddAngularImpulseInRadians(Component, FMath::DegreesToRadians(Impulse), bAccelChange);
}

FTransform ANWheelVehicle::GetVehicleTransform(UPrimitiveComponent* Component) const
{
	if (const Chaos::FRigidBodyHandle_Internal* RigidHandle = GetInternalHandle(Component))
	{
		const Chaos::FRigidTransform3 WorldCOM = Chaos::FParticleUtilitiesGT::GetActorWorldTransform(RigidHandle);
		return WorldCOM;
	}
	return Component ? Component->GetComponentTransform() : FTransform();
}

FVector ANWheelVehicle::GetLinearVelocity(UPrimitiveComponent* Component) const
{
	if (const Chaos::FRigidBodyHandle_Internal* RigidHandle = GetInternalHandle(Component))
	{
		return RigidHandle->V();
	}
	return FVector::ZeroVector;
}

FVector ANWheelVehicle::GetAngularVelocity(UPrimitiveComponent* Component) const
{
	if (const Chaos::FRigidBodyHandle_Internal* RigidHandle = GetInternalHandle(Component))
	{
		return RigidHandle->W();
	}
	return FVector::ZeroVector;
}

void ANWheelVehicle::SetLinearVelocity(UPrimitiveComponent* Component, FVector Velocity, bool bAddToCurrent)
{
	if (Chaos::FRigidBodyHandle_Internal* RigidHandle = GetInternalHandle(Component))
	{
		if (bAddToCurrent)
		{
			RigidHandle->SetV(RigidHandle->V() + Velocity);
		}
		else
		{
			RigidHandle->SetV(Velocity);
		}
	}
}

void ANWheelVehicle::SetAngularVelocityInRadians(UPrimitiveComponent* Component, FVector AngVelocity, bool bAddToCurrent)
{
	if (Chaos::FRigidBodyHandle_Internal* RigidHandle = GetInternalHandle(Component))
	{
		if (bAddToCurrent)
		{
			RigidHandle->SetW(RigidHandle->W() + AngVelocity);
		}
		else
		{
			RigidHandle->SetW(AngVelocity);
		}
	}
}

void ANWheelVehicle::SetAngularVelocityInDegrees(UPrimitiveComponent* Component, FVector AngVelocity, bool bAddToCurrent)
{
	SetAngularVelocityInRadians(Component, FMath::DegreesToRadians(AngVelocity), bAddToCurrent);
}

void ANWheelVehicle::SetWorldLocation(USceneComponent* Component, FVector Location)
{
	//check(Component);
	if (Component)
	{
		if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Component))
		{
			if (Chaos::FRigidBodyHandle_Internal* RigidHandle = GetInternalHandle(PrimitiveComponent))
			{
				const Chaos::FVec3 P = Location - RigidHandle->R().RotateVector(RigidHandle->CenterOfMass());
				RigidHandle->SetX(P);
			}
		}
		else Component->SetWorldLocation(Location);
	}
}

void ANWheelVehicle::SetWorldRotation(UPrimitiveComponent* Component, FRotator Rotation)
{
	if (Chaos::FRigidBodyHandle_Internal* RigidHandle = GetInternalHandle(Component))
	{
		const Chaos::FRotation3 Q = Rotation.Quaternion() * RigidHandle->RotationOfMass().Inverse();
		RigidHandle->SetR(Q);
	}
}

void ANWheelVehicle::SetWorldLocationAndRotation(UPrimitiveComponent* Component, FVector Location, FRotator Rotation)
{
	if (Chaos::FRigidBodyHandle_Internal* RigidHandle = GetInternalHandle(Component))
	{
		const Chaos::FRotation3 Q = Rotation.Quaternion() * RigidHandle->RotationOfMass().Inverse();
		const Chaos::FVec3 P = Location - Q.RotateVector(RigidHandle->CenterOfMass());
		RigidHandle->SetR(Q);
		RigidHandle->SetX(P);
	}
}

FVector ANWheelVehicle::GetLinearVelocityAtPoint(UPrimitiveComponent* Component, FVector Point) const
{
	if (Chaos::FRigidBodyHandle_Internal* RigidHandle = GetInternalHandle(Component))
	{
		if (ensure(RigidHandle->CanTreatAsKinematic()))
		{
			const bool bIsRigid = RigidHandle->CanTreatAsRigid();
			const Chaos::FVec3 COM = bIsRigid ? Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(RigidHandle) : static_cast<Chaos::FVec3>(Chaos::FParticleUtilitiesGT::GetActorWorldTransform(RigidHandle).GetTranslation());
			const Chaos::FVec3 Diff = Point - COM;
			return RigidHandle->V() - Chaos::FVec3::CrossProduct(Diff, RigidHandle->W());
		}
	}
	return FVector::ZeroVector;
}

Chaos::FRigidBodyHandle_Internal* ANWheelVehicle::GetInternalHandle(UPrimitiveComponent* Component)
{
	if (IsValid(Component))
	{
		if (const FBodyInstance* BodyInstance = Component->GetBodyInstance())
		{
			if (const auto Handle = BodyInstance->ActorHandle)
			{
				if (Chaos::FRigidBodyHandle_Internal* RigidHandle = Handle->GetPhysicsThreadAPI())
				{
					return RigidHandle;
				}
			}
		}
	}
	return nullptr;
}