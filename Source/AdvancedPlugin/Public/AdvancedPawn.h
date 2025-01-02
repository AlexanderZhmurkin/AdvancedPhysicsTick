// Copyright Alexander. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PBDRigidsSolver.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "GameFramework/Pawn.h"
#include "AdvancedManagerAsyncCallback.h"
#include "AdvancedPawn.generated.h"

UCLASS(BlueprintType)
class ADVANCEDPLUGIN_API AAdvancedPawn : public APawn
{
	GENERATED_BODY()
public:
	AAdvancedPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface
	
	/**
	 * Override this function to implement custom logic to be executed every physics step.
	 *
	 * @param DeltaTime - Physics step delta time
	 * @param SimTime - Total sim time since the sim began.
	 */
	virtual void TickAsync(float DeltaTime, float SimTime);

protected:
	/** Event called every advanced physics tick */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Advanced Tick"))
	void AdvancedTick(float DeltaTime, float SimTime);

public:
	/**
	 *	Add a force to a single rigid body.
	 *  This is like a 'thruster'. Good for adding a burst over some (non zero) time. Should be called every frame for the duration of the force.
	 *  CALL THIS FUNCTION ONLY AdvancedTick or TickAsync!
	 *
	 *	@param	InForce			Force vector to apply. Magnitude indicates strength of force.
	 *  @param  bAccelChange	If true, Force is taken as a change in acceleration instead of a physical force (i.e. mass will have no effect).
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvancedTick|Physics")
	void AddForce(const UPrimitiveComponent* InComponent, FVector InForce, bool bAccelChange = false);

	/**
	 *	Add a force to a single rigid body at a particular location in world space.
	 *  This is like a 'thruster'. Good for adding a burst over some (non zero) time. Should be called every frame for the duration of the force.
	 *  CALL THIS FUNCTION ONLY AdvancedTick or TickAsync!
	 *
	 *	@param Force			Force vector to apply. Magnitude indicates strength of force.
	 *	@param Location			Location to apply force, in world space.
	 *	@param bIsLocalForce	If a SkeletalMeshComponent, name of body to apply force to. 'None' indicates root body.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvancedTick|Physics")
	void AddForceAtLocation(const UPrimitiveComponent* InComponent, FVector Force, FVector Position, bool bIsLocalForce = false);

	/**
	 *	Add a torque to a single rigid body.
	 *  CALL THIS FUNCTION ONLY AdvancedTick or TickAsync!
	 * 
	 *	@param Torque		Torque to apply. Direction is axis of rotation and magnitude is strength of torque.
	 *	@param BoneName		If a SkeletalMeshComponent, name of body to apply torque to. 'None' indicates root body.
	 *  @param bAccelChange If true, Torque is taken as a change in angular acceleration instead of a physical torque (i.e. mass will have no effect).
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvancedTick|Physics")
	void AddTorque(const UPrimitiveComponent* InComponent, FVector Torque, bool bAccelChange = false);

	/**
	 *	Add an impulse to a single rigid body. Good for one time instant burst.
	 *  CALL THIS FUNCTION ONLY AdvancedTick or TickAsync!
	 *
	 *	@param	Impulse		Magnitude and direction of impulse to apply.
	 *	@param	BoneName	If a SkeletalMeshComponent, name of body to apply impulse to. 'None' indicates root body.
	 *	@param	bVelChange	If true, the Strength is taken as a change in velocity instead of an impulse (ie. mass will have no effect).
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvancedTick|Physics")
	void AddImpulse(const UPrimitiveComponent* InComponent, FVector Impulse, bool bVelChange = false);

	/**
	 *	Add an impulse to a single rigid body at a specific location.
	 *  CALL THIS FUNCTION ONLY AdvancedTick or TickAsync!
	 *
	 *	@param	Impulse		Magnitude and direction of impulse to apply.
	 *	@param	Location	Point in world space to apply impulse at.
	 *	@param	BoneName	If a SkeletalMeshComponent, name of bone to apply impulse to. 'None' indicates root body.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvancedTick|Physics")
	void AddImpulseAtLocation(const UPrimitiveComponent* InComponent, FVector Impulse, FVector Position);

	/* Get the current component-to-world transform for this component */
	UFUNCTION(BlueprintCallable, Category = "AdvancedTick|Physics", meta = (DisplayName = "Get World Transform"))
	FTransform GetPrimitiveWorldTransform(const UPrimitiveComponent* InComponent) const;

	/**
	 *	Get the linear velocity of a single body.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvancedTick|Physics")
	FVector GetLinearVelocity(const UPrimitiveComponent* InComponent);

	/**
	 *	Get the linear velocity of a point on a single body.
	 *	@param Point			Point is specified in world space.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvancedTick|Physics")
	FVector GetLinearVelocityAtPoint(const UPrimitiveComponent* InComponent, FVector Point);

	/*
	 *	Get the center of mass in primitive component
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvancedTick|Physics", meta = (DisplayName = "Get Center of Mass"))
	FVector GetPrimitiveCOM(const UPrimitiveComponent* InComponent) const;

public:

private:
	UWorld* World;
	FAdvancedManagerAsyncCallback* AsyncCallback;
};