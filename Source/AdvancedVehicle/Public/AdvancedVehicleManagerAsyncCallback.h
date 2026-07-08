// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsPublic.h"
#include "Physics/NetworkPhysicsComponent.h"
#include "Chaos/GeometryParticlesfwd.h"
#include "Chaos/SimCallbackInput.h"
#include "Chaos/SimCallbackObject.h"
#include "AdvancedVehicleUtility.h"

DECLARE_STATS_GROUP(TEXT("AdvancedVehicleManagerAsync"), STATGROUP_AdvancedVehicleManagerAsync, STATGROUP_Advanced);

struct FAdvancedVehicleManagerAsyncInput
{
public:
	FAdvancedVehicleManagerAsyncInput()
		: Vehicle(nullptr)
		, Proxy(nullptr)
	{
	}

	virtual ~FAdvancedVehicleManagerAsyncInput() = default;

	virtual void ApplySimulate(UWorld* InWorld, const float InDeltaTime, const float InSimTime) const;
	virtual void ApplyDeferredForces(Chaos::FRigidBodyHandle_Internal* InRigidHandle) const;

	FAdvancedVehicleInput PhysicsInput;
	class AAdvancedVehiclePawn* Vehicle;
	Chaos::FSingleParticlePhysicsProxy* Proxy;
};

/** GameThread -> PhysThread */
struct FAdvancedVehicleManagerAsyncCallbackInput : public Chaos::FSimCallbackInput
{
	int32 Timestamp = INDEX_NONE;
	TArray<TUniquePtr<FAdvancedVehicleManagerAsyncInput>> VehicleInputs;
	TWeakObjectPtr<UWorld> World;

	void Reset()
	{
		VehicleInputs.Reset();
		World.Reset();
	}
};

/** PhysThread -> GameThread */
struct FAdvancedVehicleManagerAsyncCallbackOutput : public Chaos::FSimCallbackOutput
{
	int32 Timestamp = INDEX_NONE;
	TArray<FAdvancedVehicleOutput> VehicleOutputs;

	void Reset()
	{
		VehicleOutputs.Reset();
	}
};

/**
 *	Asynchronous callback from the physics engine where we can simulate our vehicle.
 */
class ADVANCEDVEHICLE_API FAdvancedVehicleManagerAsyncCallback : public Chaos::TSimCallbackObject<FAdvancedVehicleManagerAsyncCallbackInput, FAdvancedVehicleManagerAsyncCallbackOutput, Chaos::ESimCallbackOptions::Presimulate | Chaos::ESimCallbackOptions::Rewind>
{
public:
	virtual FName GetFNameForStatId() const override;

private:
	virtual void ProcessInputs_Internal(int32 PhysicsStep) override;
	virtual void OnPreSimulate_Internal() override;
};