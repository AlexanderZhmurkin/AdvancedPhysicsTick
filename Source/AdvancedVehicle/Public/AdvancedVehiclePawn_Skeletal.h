// Fill out your copyright notice in the Description page of Project Settings.
// Created by: Alexander Zhmurkin

// Template (example) class for using the Skeletal Mesh Component
// It is optional class and can be overridden

#pragma once

#include "AdvancedVehiclePawn.h"
#include "AdvancedVehiclePawn_Skeletal.generated.h"

UCLASS()
class ADVANCEDVEHICLE_API AAdvancedVehiclePawn_Skeletal : public AAdvancedVehiclePawn
{
	GENERATED_BODY()
public:
	AAdvancedVehiclePawn_Skeletal(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// You can override methods:
	// Tick - for visuals
	// PhysicTick - for physics
	
	//~ Begin AActor Interface
	//virtual void Tick(float DeltaTime) override;
	//~ End AActor Interface

	//~ Begin AAdvancedVehiclePawn Interface
	//virtual void PhysicsTick(UWorld* InWorld, float DeltaTime, float SimTime, Chaos::FRigidBodyHandle_Internal* InHandle);
	//~ End AAdvancedVehiclePawn Interface
};