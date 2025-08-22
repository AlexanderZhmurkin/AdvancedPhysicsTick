// Fill out your copyright notice in the Description page of Project Settings.
// Created by: Alexander Zhmurkin

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FAdvancedPhysicsModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void PhysSceneInit(FPhysScene* PhysScene);
	void PhysSceneTerm(FPhysScene* PhysScene);

	FDelegateHandle OnPhysSceneInitHandle;
	FDelegateHandle OnPhysSceneTermHandle;
};