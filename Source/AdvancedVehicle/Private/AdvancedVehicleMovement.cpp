// Fill out your copyright notice in the Description page of Project Settings.

#include "AdvancedVehicleMovement.h"
#include "AdvancedVehiclePawn.h"

DECLARE_CYCLE_STAT(TEXT("AdvancedVehicleMovement:TickVehicle (GT)"), STAT_AdvancedVehicleMovement_TickVehicle, STATGROUP_AdvancedVehicleMovement);
DECLARE_CYCLE_STAT(TEXT("AdvancedVehicleMovement:PhysicsTick (PT)"), STAT_AdvancedVehicleMovement_PhysicsTick, STATGROUP_AdvancedVehicleMovement);

UAdvancedVehicleMovement::UAdvancedVehicleMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable(true);

	SetIsReplicatedByDefault(true);
}

void UAdvancedVehicleMovement::TickVehicle(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_AdvancedVehicleMovement_TickVehicle);
	OnTickVehicle.Broadcast(DeltaTime);
}

void UAdvancedVehicleMovement::PhysicsTick(float DeltaTime, float SimTime, Chaos::FRigidBodyHandle_Internal* InHandle)
{
	SCOPE_CYCLE_COUNTER(STAT_AdvancedVehicleMovement_PhysicsTick);
	OnPhysicsTick.Broadcast(DeltaTime, SimTime);
}