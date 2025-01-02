// Copyright Alexander. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsPublic.h"
#include "Chaos/GeometryParticlesfwd.h"
#include "Chaos/SimCallbackInput.h"
#include "Chaos/SimCallbackObject.h"

DECLARE_STATS_GROUP(TEXT("AdvancedManager"), STATGROUP_AdvancedManager, STATGROUP_Advanced);

class FRigidBodyHandle_Internal;
class AAdvancedPawn;

/*
* Used Chaos::FSimCallbackNoInput and Chaos::FSimCallbackNoOutput
*/
class ADVANCEDPLUGIN_API FAdvancedManagerAsyncCallback : public Chaos::TSimCallbackObject<Chaos::FSimCallbackNoInput, Chaos::FSimCallbackNoOutput>
{
public:
	virtual void InitAsyncCallback(AAdvancedPawn* InPawn);

private:
	virtual void ProcessInputs_Internal(int32 PhysicsStep) override;
	virtual void OnPreSimulate_Internal() override;
	AAdvancedPawn* AdvPawn;
};