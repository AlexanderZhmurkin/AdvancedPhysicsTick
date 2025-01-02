// Copyright Alexander. All Rights Reserved.

#include "AdvancedManagerAsyncCallback.h"
#include "AdvancedPawn.h"
#include "Chaos/ParticleHandleFwd.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "PBDRigidsSolver.h"

using namespace Chaos;

DECLARE_CYCLE_STAT(TEXT("AdvancedManager:PreSimulate"), STAT_AdvancedManager_OnPreSimulate, STATGROUP_AdvancedManager);

void FAdvancedManagerAsyncCallback::InitAsyncCallback(AAdvancedPawn* InPawn)
{
	AdvPawn = InPawn;
}

void FAdvancedManagerAsyncCallback::ProcessInputs_Internal(int32 PhysicsStep)
{
	int32 PStep = PhysicsStep;
}

void FAdvancedManagerAsyncCallback::OnPreSimulate_Internal()
{
	SCOPE_CYCLE_COUNTER(STAT_AdvancedManager_OnPreSimulate);

	float DeltaTime = GetDeltaTime_Internal();
	float SimTime = GetSimTime_Internal();

	if (AdvPawn)
	{
		AdvPawn->TickAsync(DeltaTime, SimTime);
	}
}