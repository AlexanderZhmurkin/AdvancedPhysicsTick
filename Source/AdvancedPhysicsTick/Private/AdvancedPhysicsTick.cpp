// Copyright Alexander. All Rights Reserved.

#include "AdvancedPhysicsTick.h"

#define LOCTEXT_NAMESPACE "AdvancedPhysicsTickModule"

void FAdvancedPhysicsTickModule::StartupModule()
{
	//OnPhysSceneInit = FPhysicsDelegates::OnPhysSceneInit.AddRaw(this, &FExtendedAsyncPawnModule::PhysScene_OnPhysSceneInit);
	//OnPhysSceneTerm = FPhysicsDelegates::OnPhysSceneTerm.AddRaw(this, &FExtendedAsyncPawnModule::PhysScene_OnPhysSceneTerm);
}

void FAdvancedPhysicsTickModule::ShutdownModule()
{
	//FPhysicsDelegates::OnPhysSceneInit.Remove(this->OnPhysSceneInit);
	//FPhysicsDelegates::OnPhysSceneTerm.Remove(this->OnPhysSceneTerm);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAdvancedPhysicsTickModule, AdvancedPhysicsTick)