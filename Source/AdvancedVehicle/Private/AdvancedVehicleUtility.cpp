// Fill out your copyright notice in the Description page of Project Settings.

#include "AdvancedVehicleUtility.h"
#include "Chaos/Particle/ParticleUtilities.h"
#include "Chaos/Utilities.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"

static float VEHICLE_MAX_DRIFT_ANGLE = 90.0f;

float FTurboMapTable::SampleGrid(const TArray<float>& Grid, float InTurboRPM, float InMcorr, float DefaultValue) const
{
	const int32 NumRPM = TurboRPMPoints.Num();
	const int32 NumM = McorrPoints.Num();
	
	if (NumRPM < 2 || NumM < 2 || Grid.Num() != NumRPM * NumM) return DefaultValue;

	auto FindSeg = [](const TArray<float>& P, float V)
		{
			const int32 N = P.Num();
			if (V <= P[0]) return 0;
			if (V >= P[N - 1]) return N - 2;
			for (int32 i = 0; i < N - 1; ++i)
			{
				if (V >= P[i] && V <= P[i + 1]) return i;
			}
			return N - 2;
		};

	const int32 R0 = FindSeg(TurboRPMPoints, InTurboRPM);
	const int32 R1 = R0 + 1;

	const float RPM0 = TurboRPMPoints[R0];
	const float RPM1 = TurboRPMPoints[R1];
	const float TRPM = (RPM1 - RPM0) > 0.0f ? (InTurboRPM - RPM0) / (RPM1 - RPM0) : 0.0f;

	const int32 M0 = FindSeg(McorrPoints, InMcorr);
	const int32 M1 = M0 + 1;

	const float M00 = McorrPoints[M0];
	const float M11 = McorrPoints[M1];
	const float TM = (M11 - M00) > 0.0f ? (InMcorr - M00) / (M11 - M00) : 0.0f;

	const float V00 = Grid[M0 * NumRPM + R0];
	const float V10 = Grid[M0 * NumRPM + R1];
	const float V01 = Grid[M1 * NumRPM + R0];
	const float V11 = Grid[M1 * NumRPM + R1];

	const float V0 = FMath::Lerp(V00, V10, TRPM);
	const float V1 = FMath::Lerp(V01, V11, TRPM);

	return FMath::Lerp(V0, V1, TM);
}

void FAdvancedVehicleStatus::CaptureState(float DeltaTime, float GravityZ, const Chaos::FRigidBodyHandle_Internal* TargetHandle)
{
	if (!TargetHandle) return;

	const FTransform WorldTM(TargetHandle->R(), TargetHandle->X());
	VehicleWorldTransform = WorldTM;
	VehicleWorldVelocity = TargetHandle->V();
	VehicleWorldVelocityNormal = VehicleWorldVelocity.GetSafeNormal();
	VehicleWorldAngularVelocity = TargetHandle->W();
	VehicleWorldCOM = TargetHandle->CenterOfMass();

	VehicleXVector = VehicleWorldTransform.GetUnitAxis(EAxis::X);
	VehicleYVector = VehicleWorldTransform.GetUnitAxis(EAxis::Y);
	VehicleZVector = VehicleWorldTransform.GetUnitAxis(EAxis::Z);

	VehicleForwardSpeed = FVector::DotProduct(VehicleWorldVelocity, VehicleXVector);
	VehicleRightSpeed = FVector::DotProduct(VehicleWorldVelocity, VehicleYVector);

	VehicleDriftAngle = VehicleForwardSpeed > 400.0f ? FMath::Clamp(FMath::RadiansToDegrees(FMath::Atan2(VehicleRightSpeed, VehicleForwardSpeed)), -VEHICLE_MAX_DRIFT_ANGLE, VEHICLE_MAX_DRIFT_ANGLE) : 0.0f;
	VehicleDriftSide = FMath::Sign(VehicleDriftAngle);

	VehicleLocalVelocity = VehicleWorldTransform.InverseTransformVector(VehicleWorldVelocity);
	VehicleLocalAcceleration = (VehicleLocalVelocity - LastVehicleLocalVelocity) / DeltaTime;
	VehicleLocalGForce = FMath::Abs(GravityZ) > 0.0f ? VehicleLocalAcceleration / FMath::Abs(GravityZ) : FVector::ZeroVector;

	LastVehicleLocalVelocity = VehicleLocalVelocity;
}











Chaos::FRigidBodyHandle_Internal* FAdvancedVehicleForces::GetInternalHandle(UPrimitiveComponent* InComponent, FName BoneName)
{
	if (IsValid(InComponent))
	{
		if (const FBodyInstance* BodyInstance = InComponent->GetBodyInstance(BoneName))
		{
			if (Chaos::FRigidBodyHandle_Internal* const InternalHandle = BodyInstance->GetPhysicsActorHandle()->GetPhysicsThreadAPI())
			{
				return InternalHandle;
			}
		}
	}

	return nullptr;
}

FVector FAdvancedVehicleForces::GetVelocityAtPoint(const FBodyInstance* InBodyInst, const FVector& InPoint, bool bIsGT)
{
	if (ensure(InBodyInst))
	{
		if (const FPhysicsActorHandle ActorHandle = InBodyInst->ActorHandle)
		{
			if (!bIsGT)
			{
				if (Chaos::FRigidBodyHandle_Internal* RigidHandle = ActorHandle->GetPhysicsThreadAPI())
				{
					if (ensure(RigidHandle))
					{
						const Chaos::FVec3 COM = RigidHandle->X() + RigidHandle->R() * RigidHandle->CenterOfMass();
						const Chaos::FVec3 Diff = InPoint - COM;
						return RigidHandle->V() - Chaos::FVec3::CrossProduct(Diff, RigidHandle->W());
					}
				}

				return FVector::ZeroVector;
			}
			else
			{
				Chaos::FRigidBodyHandle_External& RigidHandle = ActorHandle->GetGameThreadAPI();
				const Chaos::FVec3 COM = RigidHandle.X() + RigidHandle.R() * RigidHandle.CenterOfMass();
				const Chaos::FVec3 Diff = InPoint - COM;
				return RigidHandle.V() - Chaos::FVec3::CrossProduct(Diff, RigidHandle.W());
			}
		}
	}

	return FVector::ZeroVector;
}

FVector FAdvancedVehicleForces::GetVelocityAtPoint(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FVector& InPoint)
{
	if (ensure(InRigidHandle))
	{
		const Chaos::FVec3 COM = InRigidHandle->X() + InRigidHandle->R() * InRigidHandle->CenterOfMass();
		const Chaos::FVec3 Diff = InPoint - COM;
		return InRigidHandle->V() - Chaos::FVec3::CrossProduct(Diff, InRigidHandle->W());
	}

	return FVector::ZeroVector;
}

void FAdvancedVehicleForces::AddForce(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FApplyForce_DATA& InData)
{
	if (ensure(InRigidHandle))
	{
		const Chaos::EObjectStateType ObjectState = InRigidHandle->ObjectState();

		if (CHAOS_ENSURE(ObjectState == Chaos::EObjectStateType::Dynamic || ObjectState == Chaos::EObjectStateType::Sleeping))
		{
			if (InData.bAccelChange)
			{
				const float RigidMass = InRigidHandle->M();
				const Chaos::FVec3 Acceleration = InData.Force * RigidMass;
				InRigidHandle->AddForce(Acceleration, false);
			}
			else
			{
				InRigidHandle->AddForce(InData.Force, false);
			}
		}
	}
}

void FAdvancedVehicleForces::AddForceAtLocation(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FApplyForceAtLocation_DATA& InData)
{
	if (ensure(InRigidHandle))
	{
		const Chaos::EObjectStateType ObjectState = InRigidHandle->ObjectState();

		if (CHAOS_ENSURE(ObjectState == Chaos::EObjectStateType::Dynamic || ObjectState == Chaos::EObjectStateType::Sleeping))
		{
			const Chaos::FVec3 WorldCOM = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(InRigidHandle);
			const Chaos::FVec3 WorldTorque = Chaos::FVec3::CrossProduct(InData.Location - WorldCOM, InData.Force);

			InRigidHandle->AddForce(InData.Force, false);
			InRigidHandle->AddTorque(WorldTorque, false);
		}
	}
}

void FAdvancedVehicleForces::AddForceAtLocationLocal(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FApplyForceAtLocationLocal_DATA& InData)
{
	if (ensure(InRigidHandle))
	{
		const Chaos::EObjectStateType ObjectState = InRigidHandle->ObjectState();

		if (CHAOS_ENSURE(ObjectState == Chaos::EObjectStateType::Dynamic || ObjectState == Chaos::EObjectStateType::Sleeping))
		{
			const Chaos::FVec3 WorldCOM = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(InRigidHandle);
			const Chaos::FRigidTransform3 WorldTransform = Chaos::FParticleUtilitiesGT::GetActorWorldTransform(InRigidHandle);
			const Chaos::FVec3 WorldPosition = WorldTransform.TransformPosition(InData.Location);
			
			const Chaos::FVec3 WorldForce = WorldTransform.TransformVector(InData.Force);
			const Chaos::FVec3 WorldTorque = Chaos::FVec3::CrossProduct(WorldPosition - WorldCOM, WorldForce);

			InRigidHandle->AddForce(WorldForce, false);
			InRigidHandle->AddTorque(WorldTorque, false);
		}
	}
}

void FAdvancedVehicleForces::AddTorque(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FAddTorqueInRadians_DATA& InData)
{
	if (ensure(InRigidHandle))
	{
		const Chaos::EObjectStateType ObjectState = InRigidHandle->ObjectState();

		if (CHAOS_ENSURE(ObjectState == Chaos::EObjectStateType::Dynamic || ObjectState == Chaos::EObjectStateType::Sleeping))
		{
			if (InData.bAccelChange)
			{
				InRigidHandle->AddTorque(Chaos::FParticleUtilitiesXR::GetWorldInertia(InRigidHandle) * InData.Torque, false);
			}
			else
			{
				InRigidHandle->AddTorque(InData.Torque, false);
			}
		}
	}
}

void FAdvancedVehicleForces::AddTorqueAtLocation(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FAddTorqueAtLocation_DATA& InData)
{
	if (ensure(InRigidHandle))
	{
		const Chaos::EObjectStateType ObjectState = InRigidHandle->ObjectState();

		if (CHAOS_ENSURE(ObjectState == Chaos::EObjectStateType::Dynamic || ObjectState == Chaos::EObjectStateType::Sleeping))
		{
			if (InData.Torque.IsNearlyZero()) return;

			const Chaos::FVec3 Axis = InData.Torque.GetSafeNormal();
			Chaos::FVec3 Ortho = FVector::CrossProduct(Axis, FVector::UpVector);
			Ortho.Normalize();

			const float TorqueValue = InData.Torque.Size();
			const float ForceMag = TorqueValue / (2.0f * InData.ArmLength);
			const Chaos::FVec3 Force = Ortho * ForceMag;

			auto ForceAtPosition = [&InRigidHandle](const Chaos::FVec3& Force, const Chaos::FVec3& Position)
				{
					const Chaos::FVec3 COM = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(InRigidHandle);
					InRigidHandle->AddForce(Force, false);
					InRigidHandle->AddTorque(Chaos::FVec3::CrossProduct(Position - COM, Force), false);
				};

			ForceAtPosition(Force, InData.Location + Ortho * InData.ArmLength);
			ForceAtPosition(-Force, InData.Location - Ortho * InData.ArmLength);
		}
	}
}

void FAdvancedVehicleForces::AddImpulse(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FAddImpulse_DATA& InData)
{
	if (ensure(InRigidHandle))
	{
		if (InData.bVelChange)
		{
			InRigidHandle->SetLinearImpulse(InRigidHandle->LinearImpulse() + InRigidHandle->M() * InData.Impulse, false);
		}
		else
		{
			InRigidHandle->SetLinearImpulse(InRigidHandle->LinearImpulse() + InData.Impulse, false);
		}
	}
}

void FAdvancedVehicleForces::AddImpulseAtLocation(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FAddImpulseAtLocation_DATA& InData)
{
	if (ensure(InRigidHandle))
	{
		const Chaos::FVec3 WorldCOM = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(InRigidHandle);
		const Chaos::FVec3 AngularImpulse = Chaos::FVec3::CrossProduct(InData.Location - WorldCOM, InData.Impulse);

		InRigidHandle->SetLinearImpulse(InRigidHandle->LinearImpulse() + InData.Impulse, false);
		InRigidHandle->SetAngularImpulse(InRigidHandle->AngularImpulse() + AngularImpulse, false);
	}
}

void FAdvancedVehicleForces::AddAngularImpulse(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FAddAngularImpulseInRadians_DATA& InData)
{
	if (ensure(InRigidHandle))
	{
		if (InData.bVelChange)
		{
			const Chaos::FVec3 AngularImpulse = Chaos::FParticleUtilitiesXR::GetWorldInertia(InRigidHandle) * InData.Impulse;
			InRigidHandle->SetAngularImpulse(InRigidHandle->AngularImpulse() + AngularImpulse, false);
		}
		else
		{
			InRigidHandle->SetAngularImpulse(InRigidHandle->AngularImpulse() + InData.Impulse, false);
		}
	}
}