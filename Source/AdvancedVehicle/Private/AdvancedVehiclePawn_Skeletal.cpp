// Fill out your copyright notice in the Description page of Project Settings.

#include "AdvancedVehiclePawn_Skeletal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/CollisionProfile.h"

AAdvancedVehiclePawn_Skeletal::AAdvancedVehiclePawn_Skeletal(const FObjectInitializer& ObjectInitializer)
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(DefaultNameAdvancedMesh);
	Mesh->BodyInstance.bSimulatePhysics = true;
	Mesh->BodyInstance.bNotifyRigidBodyCollision = true;
	Mesh->BodyInstance.bUseCCD = true;
	Mesh->BodyInstance.bIgnoreAnalyticCollisions = false;
	Mesh->SetCollisionProfileName(UCollisionProfile::Vehicle_ProfileName);
	RootComponent = Mesh;

	if (USkeletalMeshComponent* const SkeletalMesh = Cast<USkeletalMeshComponent>(Mesh))
	{
		SkeletalMesh->SetAllBodiesSimulatePhysics(true);
		SkeletalMesh->SetGenerateOverlapEvents(true);
		SkeletalMesh->SetCanEverAffectNavigation(false);
	}
}