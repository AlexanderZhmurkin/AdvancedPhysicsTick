// Copyright Alexander. All Rights Reserved.

#include "ManagerAsyncCallback.h"
#include "AdvancedVehiclePawn.h"
#include "PBDRigidsSolver.h"
#include "Chaos/ParticleHandleFwd.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"

using namespace Chaos;

DECLARE_CYCLE_STAT(TEXT("ManagerAsync:PreSimulate"), STAT_ManagerAsync_OnPreSimulate, STATGROUP_ManagerAsync);

void FManagerAsyncCallback::InitAsyncCallback(TWeakObjectPtr<AAdvancedVehiclePawn> InVehicle)
{
	//check(InVehicle);
	Vehicle = InVehicle;
}

void FManagerAsyncCallback::ProcessInputs_Internal(int32 PhysicsStep)
{
	//TODO:
	int32 PStep = PhysicsStep;
}

void FManagerAsyncCallback::OnPreSimulate_Internal()
{
	SCOPE_CYCLE_COUNTER(STAT_ManagerAsync_OnPreSimulate);

	float DeltaTime = GetDeltaTime_Internal();
	float SimTime = GetSimTime_Internal();

	if (Vehicle == nullptr) return;
	Vehicle->TickAsync(DeltaTime, SimTime);
}