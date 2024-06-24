// Copyright Alexander. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PBDRigidsSolver.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "GameFramework/Pawn.h"
#include "NWheelVehicle.generated.h"

namespace Chaos
{
	class FRigidBodyHandle_Internal;
}

/*
* Chaos::FSimCallbackNoInput - �� ������������ ��� �����
* Chaos::FSimCallbackNoOutput - �� ������������ ��� ������
* ������ Chaos::FSimCallbackInput � Chaos::FSimCallbackOutput
*/
class FExtendedAsyncPawn : public Chaos::TSimCallbackObject<Chaos::FSimCallbackNoInput, Chaos::FSimCallbackNoOutput>
{
public:
	void Initialize(ANWheelVehicle* InVehicle);
private:
	virtual void OnPreSimulate_Internal() override;
	ANWheelVehicle* Vehicle;
};

UCLASS(BlueprintType)
class EXTENDEDASYNCPAWN_API ANWheelVehicle : public APawn
{
	GENERATED_BODY()
public:
	ANWheelVehicle(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void TickAsync(float DeltaTime, float SimTime);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "NWheelVehicle")
	void AsyncTick(float DeltaTime, float SimTime);

	/** Add a Force to this NWheelVehicle */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	void AddForce(UPrimitiveComponent* Component, FVector Force, bool bAccelChange = false);
	/** Add a Force at a particular position to this NWheelVehicle */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	void AddForceAtPosition(UPrimitiveComponent* Component, FVector Force, FVector Position, bool bIsLocalForce = false);
	/** Add a Torque to this NWheelVehicle */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	void AddTorque(UPrimitiveComponent* Component, FVector Torque, bool bAccelChange = false);
	/** Add an Impulse to this NWheelVehicle */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	void AddImpulse(UPrimitiveComponent* Component, FVector Impulse, bool bVelChange);
	/** Add an Impulse to this NWheelVehicle and a particular world position */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	void AddImpulseAtPosition(UPrimitiveComponent* Component, FVector Impulse, FVector Position);
	/** Add an Angular impulse in Rad to this NWheelVehicle */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	void AddAngularImpulseInRadians(UPrimitiveComponent* Component, FVector Impulse, bool bAccelChange = false);
	/** Add an Angular impulse in Degrees to this NWheelVehicle */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	void AddAngularImpulseInDegrees(UPrimitiveComponent* Component, FVector Impulse, bool bAccelChange = false);
	/** Add an Transform to this NWheelVehicle */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	FTransform GetVehicleTransform(UPrimitiveComponent* Component) const;
	/** For NWheelVehicle */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	FVector GetLinearVelocity(UPrimitiveComponent* Component) const;
	/** For NWheelVehicle */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	FVector GetAngularVelocity(UPrimitiveComponent* Component) const;
	/** For NWheelVehicle */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	void SetLinearVelocity(UPrimitiveComponent* Component, FVector Velocity, bool bAddToCurrent);
	/** For NWheelVehicle */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	void SetAngularVelocityInRadians(UPrimitiveComponent* Component, FVector AngVelocity, bool bAddToCurrent);
	/** For NWheelVehicle */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	void SetAngularVelocityInDegrees(UPrimitiveComponent* Component, FVector AngVelocity, bool bAddToCurrent);
	/** For NWheelVehicle */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	void SetWorldLocation(USceneComponent* Component, FVector Location);
	/** For NWheelVehicle */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	void SetWorldRotation(UPrimitiveComponent* Component, FRotator Rotation);
	/** For NWheelVehicle */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	void SetWorldLocationAndRotation(UPrimitiveComponent* Component, FVector Location, FRotator Rotation);
	/** For NWheelVehicle */
	UFUNCTION(BlueprintCallable, Category = "NWheelVehicle")
	FVector GetLinearVelocityAtPoint(UPrimitiveComponent* Component, FVector Point) const;

private:
	static Chaos::FRigidBodyHandle_Internal* GetInternalHandle(UPrimitiveComponent* Component);
	FExtendedAsyncPawn* AsyncCallback;
};