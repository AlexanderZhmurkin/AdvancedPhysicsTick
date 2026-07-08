// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsPublic.h"
#include "Chaos/GeometryParticlesfwd.h"
#include "Chaos/SimCallbackInput.h"
#include "Chaos/SimCallbackObject.h"
#include "AdvancedVehicleManagerAsyncCallback.h"

DECLARE_STATS_GROUP(TEXT("AdvancedVehicle"), STATGROUP_AdvancedVehicle, STATGROUP_Advanced);

class AAdvancedVehiclePawn;
class FChaosScene;

class ADVANCEDVEHICLE_API FAdvancedVehicleManager
{
public:
	FAdvancedVehicleManager(FPhysScene* PhysScene);
	~FAdvancedVehicleManager();

	static void OnPostWorldInitialization(UWorld* InWorld, const UWorld::InitializationValues);
	static void OnWorldCleanup(UWorld* InWorld, bool bSessionEnded, bool bCleanupResources);

	void RegisterCallbacks();
	void UnregisterCallbacks();

	/** Scene.OnPhysScenePreTick */
	void PreUpdate(FPhysScene* PhysScene, float DeltaTime);
	/** Scene.OnPhysScenePostTick */
	void PostUpdate(FChaosScene* PhysScene);

	/** Add vehicle to vehicle manager */
	void AddVehicle(TWeakObjectPtr<AAdvancedVehiclePawn> InVehicle);
	/** Add vehicle from vehicle manager */
	void RemoveVehicle(TWeakObjectPtr<AAdvancedVehiclePawn> InVehicle);

	/** Find a manager callback from an FPhysScene */
	static FAdvancedVehicleManager* GetManagerFromScene(FPhysScene* InPhysScene);
	/** Detach manager from a FPhysScene (remove delegates, remove from map etc) */
	void DetachManagerFromPhysScene(FPhysScene* InPhysScene);

	FPhysScene_Chaos& GetScene() const { return Scene; }
	static uint32 VehicleSetupTag;

private:
	static bool bInitialized;
	FPhysScene_Chaos& Scene;

	static TMap<FPhysScene*, FAdvancedVehicleManager*> SceneToPhysicsManagerMap;
	TArray<TWeakObjectPtr<AAdvancedVehiclePawn>> Vehicles;

	FDelegateHandle OnPhysScenePreTickHandle;
	FDelegateHandle OnPhysScenePostTickHandle;

	static FDelegateHandle OnPostWorldInitHandle;
	static FDelegateHandle OnWorldCleanupHandle;

	FAdvancedVehicleManagerAsyncCallback* AsyncCallback;
	int32 Timestamp;
	int32 SleepingCount;
	int32 SubstepCount;
};