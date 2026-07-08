// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AdvancedVehicleUtility.h"
#include "AdvancedVehiclePawn.generated.h"

class UAdvancedVehicleMovement;
class UPrimitiveComponent;

struct FAdvancedVehicleManagerAsyncInput;
struct FAdvancedVehicleManagerAsyncCallbackInput;

DECLARE_STATS_GROUP(TEXT("AdvancedVehiclePawn"), STATGROUP_AdvancedVehiclePawn, STATGROUP_Advanced);

/**
 *	AAdvancedVehiclePawn is the base pawn for single physics body wheeled vehicle pawn.
 *	You can override this class for StaticMesh or SkeletalMesh, (see template class AAdvancedVehiclePawn_Skeletal and AAdvancedVehiclePawn_Static)
 * 
 *	By default you can use optional UAdvancedVehicleMovement class for engine, transmission and differentials, but this can be overridden by inheriting from the class and modifying its constructor like so
 *	Super(ObjectInitializer.SetDefaultSubobjectClass<UMyVehicleMovementComponent>())
 */
UCLASS(Abstract, BlueprintType, HideCategories = (HLOD, WorldPartition))
class ADVANCEDVEHICLE_API AAdvancedVehiclePawn : public APawn
{
	friend class FAdvancedVehicleManager;
	friend class FAdvancedVehicleAsyncCallback;

	GENERATED_BODY()
public:
	AAdvancedVehiclePawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitializeComponents() override;
	//~ End AActor Interface

	//~ Begin APawn Interface
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	//~ End APawn Interface

	/** Tick rate physics for PHYSIC_THREAD */
	virtual void PhysicsTick(UWorld* InWorld, float DeltaTime, float SimTime, Chaos::FRigidBodyHandle_Internal* InHandle);
	/** Build async input for async manager */
	virtual TUniquePtr<FAdvancedVehicleManagerAsyncInput> BuildAsyncInput(int32 InputIdx, int32 Timestamp);

protected:
	/** Event called PHYSIC_THREAD */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Advanced Vehicle Physic Tick"))
	void BP_PhysicsTick(float DeltaTime, float SimTime);

	virtual void ThrottleActionBinding(const struct FInputActionValue& ActionValue);
	virtual void SteeringActionBinding(const struct FInputActionValue& ActionValue);
	virtual void BrakingActionBinding(const struct FInputActionValue& ActionValue);
	virtual void HandbrakeActionBinding(const struct FInputActionValue& ActionValue);
	virtual void ClutchActionBinding(const struct FInputActionValue& ActionValue);
	
public:
	/** Returns Body Mesh subobject **/
	class UPrimitiveComponent* GetMesh() const { return Mesh; }
	/** Returns Advanced Movement subobject **/
	class UAdvancedVehicleMovement* GetAdvancedMovement() const { return AdvancedMovement; }

	/** Returns the body instance of the vehicle mesh. */
	FBodyInstance* GetBodyInstance() { return Mesh->GetBodyInstance(); }
	/** Returns the body instance of the vehicle mesh (const version). */
	const FBodyInstance* GetBodyInstance() const { return Mesh->GetBodyInstance(); }
	/** Returns the Chaos rigid body handle for the vehicle. */
	Chaos::FRigidBodyHandle_Internal* GetRigidHandle();
	/** Returns the Chaos rigid body handle for the vehicle (const version). */
	const Chaos::FRigidBodyHandle_Internal* GetRigidHandle() const;

protected:
	/** Main physics body skeletal or static mesh associated with this advanced vehicle */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Advanced Vehicle", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPrimitiveComponent> Mesh = nullptr;

	/** Advanced simulation component for mechanical simulation (optional) */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Advanced Vehicle", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAdvancedVehicleMovement> AdvancedMovement = nullptr;

public:
	/** Applies custom vehicle forces to the specified rigid body handle. */
	void ApplyVehicleForces(Chaos::FRigidBodyHandle_Internal* InHandle);

	/** Applies a force to the advanced vehicle. */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Physics")
	void AddForce(const FVector& Force, bool bAccelChange = false);

	/** Applies a force at a specific location on the advanced vehicle. */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Physics")
	void AddForceAtLocation(const FVector& Force, const FVector& Location, bool bIsLocalForce = false);

	/** Applies torque (in radians) to the advanced vehicle. */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Physics")
	void AddTorqueInRadians(const FVector& Torque, bool bAccelChange = false);

	/** Applies torque at a specific location with arm length. */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Physics")
	void AddTorqueAtLocation(const FVector& Torque, const FVector& Location, float ArmLength, bool bAccelChange = false);

	/** Applies an impulse to the advanced vehicle. */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Physics")
	void AddImpulse(const FVector& Impulse, bool bVelChange = false);

	/** Applies an impulse at a specific location on the advanced vehicle. */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Physics")
	void AddImpulseAtLocation(const FVector& Impulse, const FVector& Location);

	/** Applies an angular impulse (in radians) to the advanced vehicle. */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Physics")
	void AddAngularImpulseInRadians(const FVector& Impulse, bool bVelChange);

	/** Returns the linear velocity at a specific point on the target component. */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Physics")
	FVector GetLinearVelocityAtPoint(const UPrimitiveComponent* TargetComponent, const FVector& InPoint);

public:
	/** Set the user input for the vehicle throttle [range 0 to 1] */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Input")
	void SetThrottleInput(float InInput);

	/** Set the user input for the vehicle steering [range -1 to 1] */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Input")
	void SetSteeringInput(float InInput);

	/** Set the user input for the vehicle braking [range 0 to 1] */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Input")
	void SetBrakingInput(float InInput);

	/** Set the user input for the vehicle handbrake [range 0 to 1] */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Input")
	void SetHandbrakeInput(float InInput);

	/** Set the user input for the vehicle clutch [range 0 to 1] */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Input")
	void SetClutchInput(float InInput);

public:
	/** Returns advanced vehicle world transform */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|State")
	FTransform GetVehicleWorldTransform() const { return VehicleState.VehicleWorldTransform; }

	/** Returns advanced vehicle world velocity */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|State")
	FVector GetVehicleWorldVelocity() const { return VehicleState.VehicleWorldVelocity; }

	/** Returns vehicle world velocity size */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|State")
	float GetVehicleWorldVelocitySize() const { return VehicleState.VehicleWorldVelocity.Size(); }

	/** Returns vehicle world velocity normal */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|State")
	FVector GetVehicleWorldVelocityNormal() const { return VehicleState.VehicleWorldVelocityNormal; }

	/** Returns vehicle world velocity normal */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|State")
	FVector GetVehicleWorldAngularVelocity() const { return VehicleState.VehicleWorldAngularVelocity; }

	/** Returns vehicle world center of mass */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|State")
	FVector GetVehicleWorldCOM() const { return VehicleState.VehicleWorldCOM; }

	/** Returns vehicle X vector */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|State")
	FVector GetVehicleXVector() const { return VehicleState.VehicleXVector; }

	/** Returns vehicle Y vector */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|State")
	FVector GetVehicleYVector() const { return VehicleState.VehicleYVector; }

	/** Returns vehicle Z vector */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|State")
	FVector GetVehicleZVector() const { return VehicleState.VehicleZVector; }

	/** Returns vehicle forward speed */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|State")
	float GetVehicleForwardSpeed() const { return VehicleState.VehicleForwardSpeed; }

	/** Returns vehicle right speed */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|State")
	float GetVehicleRightSpeed() const { return VehicleState.VehicleRightSpeed; }

	/** Returns vehicle drift angle */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|VehicleState")
	float GetVehicleDriftAngle() const { return VehicleState.VehicleDriftAngle; }

	/** Returns vehicle drift side */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|VehicleState")
	float GetVehicleDriftSide() const { return VehicleState.VehicleDriftSide; }

	/** Returns vehicle local velocity */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|State")
	FVector GetVehicleLocalVelocity() const { return VehicleState.VehicleLocalVelocity; }

	/** Returns vehicle local acceleration */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|State")
	FVector GetVehicleLocalAcceleration() const { return VehicleState.VehicleLocalAcceleration; }

	/** Returns vehicle G local force */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|State")
	FVector GetVehicleLocalGForce() const { return VehicleState.VehicleLocalGForce; }

	/** Get the user input for the vehicle throttle [range 0 to 1] */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Input")
	float GetThrottleInput() const { return VehicleInput.ThrottleInput; }

	/** Get the user input for the vehicle steering [range -1 to 1] */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Input")
	float GetSteeringInput() const { return VehicleInput.SteeringInput; }

	/** Get the user input for the vehicle braking [range 0 to 1] */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Input")
	float GetBrakingInput() const { return VehicleInput.BrakingInput; }

	/** Get the user input for the vehicle handbrake [range 0 to 1] */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Input")
	float GetHandbrakeInput() const { return VehicleInput.HandbrakeInput; }

	/** Get the user input for the vehicle clutch [range 0 to 1] */
	UFUNCTION(BlueprintCallable, Category = "AdvancedVehiclePawn|Input")
	float GetClutchInput() const { return VehicleInput.ClutchInput; }

public:
	/** Input mapping priority when initializate */
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	int32 InputMappingPriority;

	/** Input mapping context for AVehicleRacingBase */
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	class UInputMappingContext* InputMappingContext;

	/** Action throttle pedal */
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	class UInputAction* ThrottleAction;

	/** Action steering wheel */
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	class UInputAction* SteeringAction;

	/** Action braking pedal */
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	class UInputAction* BrakingAction;

	/** Action handbrake grip */
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	class UInputAction* HandbrakeAction;

	/** Action clutch pedal */
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	class UInputAction* ClutchAction;

public:
	static FName DefaultNameAdvancedMesh;
	static FName DefaultNameAdvancedMovemenetComponent;

private:
	FAdvancedVehicleForces VehicleForce;
	FAdvancedVehicleStatus VehicleState;
	FAdvancedVehicleInput VehicleInput;

	Chaos::FRigidBodyHandle_Internal* RigidHandle;
	UWorld* CurrentWorld;
};