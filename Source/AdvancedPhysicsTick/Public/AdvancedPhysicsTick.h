// Copyright Alexander. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FAdvancedPhysicsTickModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	//void PhysScene_OnPhysSceneInit(FPhysScene* Scene);
	//void PhysScene_OnPhysSceneTerm(FPhysScene* Scene);
	//FDelegateHandle OnPhysSceneInit;
	//FDelegateHandle OnPhysSceneTerm;
};