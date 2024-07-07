// Copyright Alexander. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsPublic.h"
#include "Chaos/GeometryParticlesfwd.h"
#include "Chaos/SimCallbackInput.h"
#include "Chaos/SimCallbackObject.h"

DECLARE_STATS_GROUP(TEXT("ManagerAsync"), STATGROUP_ManagerAsync, STATGROUP_Advanced);

class AAdvancedVehiclePawn;

namespace Chaos
{
	class FRigidBodyHandle_Internal;
}

/*
* Use Chaos::FSimCallbackNoInput
* Use Chaos::FSimCallbackNoOutput
* For Chaos::FSimCallbackInput and Chaos::FSimCallbackOutput
*/
class ADVANCEDPHYSICSTICK_API FManagerAsyncCallback : public Chaos::TSimCallbackObject<Chaos::FSimCallbackNoInput, Chaos::FSimCallbackNoOutput>
{
public:
	virtual void InitAsyncCallback(TWeakObjectPtr<AAdvancedVehiclePawn> InVehicle);
private:
	virtual void ProcessInputs_Internal(int32 PhysicsStep) override;
	virtual void OnPreSimulate_Internal() override;
	TWeakObjectPtr<AAdvancedVehiclePawn> Vehicle;
};