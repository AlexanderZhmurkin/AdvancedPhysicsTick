// Fill out your copyright notice in the Description page of Project Settings.
// Created by: Alexander Zhmurkin

#include "AdvancedPhysics.h"
#include "AdvancedManager.h"

IMPLEMENT_MODULE(FAdvancedPhysicsModule, AdvancedPhysics)

void FAdvancedPhysicsModule::PhysSceneInit(FPhysScene* PhysScene)
{
	new FAdvancedManager(PhysScene);
}

void FAdvancedPhysicsModule::PhysSceneTerm(FPhysScene* PhysScene)
{
	if (FAdvancedManager* AdvancedManager = FAdvancedManager::GetAdvancedManagerFromScene(PhysScene))
	{
		AdvancedManager->DetachFromPhysScene(PhysScene);
		delete AdvancedManager;
		AdvancedManager = nullptr;
	}
}

void FAdvancedPhysicsModule::StartupModule()
{
	OnPhysSceneInitHandle = FPhysicsDelegates::OnPhysSceneInit.AddRaw(this, &FAdvancedPhysicsModule::PhysSceneInit);
	OnPhysSceneTermHandle = FPhysicsDelegates::OnPhysSceneTerm.AddRaw(this, &FAdvancedPhysicsModule::PhysSceneTerm);
}

void FAdvancedPhysicsModule::ShutdownModule()
{
	FPhysicsDelegates::OnPhysSceneInit.Remove(OnPhysSceneInitHandle);
	FPhysicsDelegates::OnPhysSceneTerm.Remove(OnPhysSceneTermHandle);
}