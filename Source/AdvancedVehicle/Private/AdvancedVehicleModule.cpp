// Fill out your copyright notice in the Description page of Project Settings.

#include "AdvancedVehicleModule.h"
#include "AdvancedVehicleManager.h"

IMPLEMENT_MODULE(FAdvancedVehicleModule, AdvancedVehicle)

void FAdvancedVehicleModule::PhysSceneInit(FPhysScene* PhysScene)
{
	SceneToManagerMap.Add(PhysScene, MakeUnique<FAdvancedVehicleManager>(PhysScene));
}

void FAdvancedVehicleModule::PhysSceneTerm(FPhysScene* PhysScene)
{
	SceneToManagerMap.Remove(PhysScene);
}

void FAdvancedVehicleModule::StartupModule()
{
	OnPhysSceneInitHandle = FPhysicsDelegates::OnPhysSceneInit.AddRaw(this, &FAdvancedVehicleModule::PhysSceneInit);
	OnPhysSceneTermHandle = FPhysicsDelegates::OnPhysSceneTerm.AddRaw(this, &FAdvancedVehicleModule::PhysSceneTerm);
}

void FAdvancedVehicleModule::ShutdownModule()
{
	FPhysicsDelegates::OnPhysSceneInit.Remove(OnPhysSceneInitHandle);
	FPhysicsDelegates::OnPhysSceneTerm.Remove(OnPhysSceneTermHandle);
}