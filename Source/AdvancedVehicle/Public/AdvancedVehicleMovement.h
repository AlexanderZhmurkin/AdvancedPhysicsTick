// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "AdvancedVehicleMovement.generated.h"

DECLARE_STATS_GROUP(TEXT("AdvancedVehicleMovement"), STATGROUP_AdvancedVehicleMovement, STATGROUP_Advanced);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAdvancedVehicleGameCallbackSignature, float, DeltaTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAdvancedVehiclePhysCallbackSignature, float, DeltaTime, float, SimTime);

UCLASS(meta = (BlueprintSpawnableComponent), HideCategories = (ComponentTick, PlanarMovement, Activation, Cooking))
class ADVANCEDVEHICLE_API UAdvancedVehicleMovement : public UPawnMovementComponent
{
	friend class FAdvancedVehicleManager;
	friend class FAdvancedVehicleManagerAsyncCallback;

	GENERATED_UCLASS_BODY()
public:
	/** Game thread */
	virtual void TickVehicle(float DeltaTime);
	/** Physics thread */
	virtual void PhysicsTick(float DeltaTime, float SimTime, Chaos::FRigidBodyHandle_Internal* InHandle);

public:
	/** Called on game thread update */
	UPROPERTY(BlueprintAssignable)
	FAdvancedVehicleGameCallbackSignature OnTickVehicle;

	/** Called on physic thread update */
	UPROPERTY(BlueprintAssignable)
	FAdvancedVehiclePhysCallbackSignature OnPhysicsTick;
};