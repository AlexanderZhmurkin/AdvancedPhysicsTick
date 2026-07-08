// Fill out your copyright notice in the Description page of Project Settings.

#include "AdvancedVehiclePawn_Static.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"

AAdvancedVehiclePawn_Static::AAdvancedVehiclePawn_Static(const FObjectInitializer& ObjectInitializer)
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(DefaultNameAdvancedMesh);
	Mesh->BodyInstance.bSimulatePhysics = true;
	Mesh->BodyInstance.bNotifyRigidBodyCollision = true;
	Mesh->BodyInstance.bUseCCD = true;
	Mesh->BodyInstance.bIgnoreAnalyticCollisions = false;
	Mesh->SetCollisionProfileName(UCollisionProfile::Vehicle_ProfileName);
	Mesh->SetGenerateOverlapEvents(true);
	RootComponent = Mesh;
}