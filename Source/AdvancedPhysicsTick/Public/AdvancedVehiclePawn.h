// Copyright Alexander. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PBDRigidsSolver.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "GameFramework/Pawn.h"
#include "AdvancedVehiclePawn.generated.h"

class FManagerAsyncCallback;

UCLASS(BlueprintType)
class ADVANCEDPHYSICSTICK_API AAdvancedVehiclePawn : public APawn
{
	GENERATED_BODY()
public:
	AAdvancedVehiclePawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	/**
	 * Override this function to implement custom logic to be executed every physics step.
	 *	
	 * @param DeltaTime - The physics step delta time with Substepping
	 * @param SimTime - This is the total sim time since the sim began.
	 */
	virtual void TickAsync(float DeltaTime, float SimTime);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Event called every advanced physics tick */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Advanced Tick"))
	void AdvancedTick(float DeltaTime, float SimTime);

	/** Add a Force to this AdvancedVehiclePawn */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	void AddForce(UPrimitiveComponent* Component, FVector Force, bool bAccelChange = false);
	/** Add a Force at a particular position to this AdvancedVehiclePawn */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	void AddForceAtPosition(UPrimitiveComponent* Component, FVector Force, FVector Position, bool bIsLocalForce = false);
	/** Add a Torque to this AdvancedVehiclePawn */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	void AddTorque(UPrimitiveComponent* Component, FVector Torque, bool bAccelChange = false);
	/** Add an Impulse to this AdvancedVehiclePawn */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	void AddImpulse(UPrimitiveComponent* Component, FVector Impulse, bool bVelChange);
	/** Add an Impulse to this AdvancedVehiclePawn and a particular world position */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	void AddImpulseAtPosition(UPrimitiveComponent* Component, FVector Impulse, FVector Position);
	/** Add an Angular impulse in Rad to this AdvancedVehiclePawn */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	void AddAngularImpulseInRadians(UPrimitiveComponent* Component, FVector Impulse, bool bAccelChange = false);
	/** Add an Angular impulse in Degrees to this AdvancedVehiclePawn */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	void AddAngularImpulseInDegrees(UPrimitiveComponent* Component, FVector Impulse, bool bAccelChange = false);
	/** Add an Transform to this AdvancedVehiclePawn */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	FTransform GetVehicleTransform(UPrimitiveComponent* Component) const;
	/** For AdvancedVehiclePawn */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	FVector GetLinearVelocity(UPrimitiveComponent* Component) const;
	/** For AdvancedVehiclePawn */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	FVector GetAngularVelocity(UPrimitiveComponent* Component) const;
	/** For AdvancedVehiclePawn */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	void SetLinearVelocity(UPrimitiveComponent* Component, FVector Velocity, bool bAddToCurrent);
	/** For AdvancedVehiclePawn */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	void SetAngularVelocityInRadians(UPrimitiveComponent* Component, FVector AngVelocity, bool bAddToCurrent);
	/** For AdvancedVehiclePawn */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	void SetAngularVelocityInDegrees(UPrimitiveComponent* Component, FVector AngVelocity, bool bAddToCurrent);
	/** For AdvancedVehiclePawn */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	void SetWorldLocation(USceneComponent* Component, FVector Location);
	/** For AdvancedVehiclePawn */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	void SetWorldRotation(UPrimitiveComponent* Component, FRotator Rotation);
	/** For AdvancedVehiclePawn */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	void SetWorldLocationAndRotation(UPrimitiveComponent* Component, FVector Location, FRotator Rotation);
	/** For AdvancedVehiclePawn */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn")
	FVector GetLinearVelocityAtPoint(UPrimitiveComponent* Component, FVector Point) const;

private:
	static Chaos::FRigidBodyHandle_Internal* GetInternalHandle(UPrimitiveComponent* Component);
	FManagerAsyncCallback* AsyncCallback;
};