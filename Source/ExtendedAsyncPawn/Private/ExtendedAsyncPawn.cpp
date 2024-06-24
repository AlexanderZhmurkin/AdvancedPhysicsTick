// Copyright Alexander. All Rights Reserved.

#include "ExtendedAsyncPawn.h"

#define LOCTEXT_NAMESPACE "FAsyncTickPhysicsModule"

void FExtendedAsyncPawnModule::StartupModule()
{
	//OnPhysSceneInit = FPhysicsDelegates::OnPhysSceneInit.AddRaw(this, &FExtendedAsyncPawnModule::PhysScene_OnPhysSceneInit);
	//OnPhysSceneTerm = FPhysicsDelegates::OnPhysSceneTerm.AddRaw(this, &FExtendedAsyncPawnModule::PhysScene_OnPhysSceneTerm);
}

void FExtendedAsyncPawnModule::ShutdownModule()
{
	//FPhysicsDelegates::OnPhysSceneInit.Remove(this->OnPhysSceneInit);
	//FPhysicsDelegates::OnPhysSceneTerm.Remove(this->OnPhysSceneTerm);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FExtendedAsyncPawnModule, ExtendedAsyncPawn)