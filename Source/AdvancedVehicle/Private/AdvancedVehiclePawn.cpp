// Fill out your copyright notice in the Description page of Project Settings.

#include "AdvancedVehiclePawn.h"
#include "AdvancedVehicleMovement.h"
#include "AdvancedVehicleManager.h"
#include "AdvancedVehicleManagerAsyncCallback.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"

DECLARE_CYCLE_STAT(TEXT("AdvancedVehiclePawn:PhysicsTick"), STAT_AdvancedVehiclePawn_PhysicsTick, STATGROUP_AdvancedVehiclePawn);

FName AAdvancedVehiclePawn::DefaultNameAdvancedMesh(TEXT("Mesh"));
FName AAdvancedVehiclePawn::DefaultNameAdvancedMovemenetComponent(TEXT("Advanced Movement Component"));

AAdvancedVehiclePawn::AAdvancedVehiclePawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// AdvancedMovement is intentionally NOT created in the base pawn.
	//
	// This pawn is designed to be modular. A movement component can be:
	//   - created here in C++ using CreateDefaultSubobject(), or
	//   - added later in a Blueprint as an optional component.
	//
	// The base class will automatically search for UAdvancedVehicleMovement
	// (PostInitializeComponents/BeginPlay) and cache a pointer to it.
	// This allows different vehicle types to use different movement components
	// without creating separate pawn hierarchies or forcing every vehicle to
	// contain the same simulation component.
	//
	// AdvancedMovement = CreateDefaultSubobject<UAdvancedVehicleMovement>(DefaultNameAdvancedMovemenetComponent);
	// AdvancedMovement->SetIsReplicated(true);
	// AdvancedMovement->UpdatedComponent = RootComponent;

	InputMappingPriority = 1;
	InputMappingContext = nullptr;
	ThrottleAction = nullptr;
	SteeringAction = nullptr;
	BrakingAction = nullptr;
	HandbrakeAction = nullptr;
	ClutchAction = nullptr;
}

void AAdvancedVehiclePawn::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld()->IsGameWorld())
	{
		if (FPhysScene* const PhysScene = GetWorld()->GetPhysicsScene())
		{
			if (FAdvancedVehicleManager* Manager = FAdvancedVehicleManager::GetManagerFromScene(PhysScene))
			{
				Manager->AddVehicle(this);
			}
		}
	}
}

void AAdvancedVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(AdvancedMovement))
	{
		AdvancedMovement->TickVehicle(DeltaTime);
	}
}

void AAdvancedVehiclePawn::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	FAdvancedVehicleManager* VehicleManager = FAdvancedVehicleManager::GetManagerFromScene(GetWorld()->GetPhysicsScene());
	VehicleManager->RemoveVehicle(this);
}

void AAdvancedVehiclePawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AdvancedMovement = FindComponentByClass<UAdvancedVehicleMovement>();
}

void AAdvancedVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* const EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ThisClass::ThrottleActionBinding);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ThisClass::ThrottleActionBinding);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ThisClass::SteeringActionBinding);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ThisClass::SteeringActionBinding);
		EnhancedInputComponent->BindAction(BrakingAction, ETriggerEvent::Triggered, this, &ThisClass::BrakingActionBinding);
		EnhancedInputComponent->BindAction(BrakingAction, ETriggerEvent::Completed, this, &ThisClass::BrakingActionBinding);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Triggered, this, &ThisClass::HandbrakeActionBinding);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ThisClass::HandbrakeActionBinding);
		EnhancedInputComponent->BindAction(ClutchAction, ETriggerEvent::Triggered, this, &ThisClass::ClutchActionBinding);
		EnhancedInputComponent->BindAction(ClutchAction, ETriggerEvent::Completed, this, &ThisClass::ClutchActionBinding);
	}
}

void AAdvancedVehiclePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (const APlayerController* PC = Cast<APlayerController>(NewController))
	{
		if (PC->IsLocalPlayerController() && InputMappingContext)
		{
			if (UEnhancedInputLocalPlayerSubsystem* SubsystemInterface = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				SubsystemInterface->AddMappingContext(InputMappingContext, InputMappingPriority);
			}
		}
	}
}

void AAdvancedVehiclePawn::UnPossessed()
{
	Super::UnPossessed();

	if (const APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->IsLocalPlayerController() && InputMappingContext)
		{
			if (UEnhancedInputLocalPlayerSubsystem* SubsystemInterface = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				SubsystemInterface->RemoveMappingContext(InputMappingContext);
			}
		}
	}
}

void AAdvancedVehiclePawn::PhysicsTick(UWorld* InWorld, float DeltaTime, float SimTime, Chaos::FRigidBodyHandle_Internal* InHandle)
{
	SCOPE_CYCLE_COUNTER(STAT_AdvancedVehiclePawn_PhysicsTick);

	CurrentWorld = InWorld;
	RigidHandle = InHandle;

	if (IsValid(CurrentWorld) && RigidHandle)
	{
		VehicleState.CaptureState(DeltaTime, -980.0f, RigidHandle);
		BP_PhysicsTick(DeltaTime, SimTime);

		if (IsValid(AdvancedMovement))
		{
			AdvancedMovement->PhysicsTick(DeltaTime, SimTime, InHandle);
		}
	}
}

TUniquePtr<FAdvancedVehicleManagerAsyncInput> AAdvancedVehiclePawn::BuildAsyncInput(int32 InputIdx, int32 Timestamp)
{
	TUniquePtr<FAdvancedVehicleManagerAsyncInput> AsyncInput = MakeUnique<FAdvancedVehicleManagerAsyncInput>();
	AsyncInput->Vehicle = this;

	if (const FBodyInstance* BodyInstance = GetBodyInstance())
	{
		const FPhysicsActorHandle Handle = BodyInstance->GetPhysicsActor();
		AsyncInput->Proxy = Handle;
	}

	AsyncInput->PhysicsInput.ThrottleInput = VehicleInput.ThrottleInput;
	AsyncInput->PhysicsInput.SteeringInput = VehicleInput.SteeringInput;
	AsyncInput->PhysicsInput.BrakingInput = VehicleInput.BrakingInput;
	AsyncInput->PhysicsInput.HandbrakeInput = VehicleInput.HandbrakeInput;
	AsyncInput->PhysicsInput.ClutchInput = VehicleInput.ClutchInput;

	return AsyncInput;
}

void AAdvancedVehiclePawn::ThrottleActionBinding(const FInputActionValue& ActionValue)
{
	SetThrottleInput(ActionValue.GetMagnitude());
}

void AAdvancedVehiclePawn::SteeringActionBinding(const FInputActionValue& ActionValue)
{
	SetSteeringInput(ActionValue.GetMagnitude());
}

void AAdvancedVehiclePawn::BrakingActionBinding(const FInputActionValue& ActionValue)
{
	SetBrakingInput(ActionValue.GetMagnitude());
}

void AAdvancedVehiclePawn::HandbrakeActionBinding(const FInputActionValue& ActionValue)
{
	SetHandbrakeInput(ActionValue.GetMagnitude());
}

void AAdvancedVehiclePawn::ClutchActionBinding(const FInputActionValue& ActionValue)
{
	SetClutchInput(ActionValue.GetMagnitude());
}

Chaos::FRigidBodyHandle_Internal* AAdvancedVehiclePawn::GetRigidHandle()
{
	return RigidHandle;
}

const Chaos::FRigidBodyHandle_Internal* AAdvancedVehiclePawn::GetRigidHandle() const
{
	return RigidHandle;
}

void AAdvancedVehiclePawn::ApplyVehicleForces(Chaos::FRigidBodyHandle_Internal* InHandle)
{
	VehicleForce.Apply(InHandle);
}

void AAdvancedVehiclePawn::AddForce(const FVector& Force, bool bAccelChange)
{
	VehicleForce.Add(FAdvancedVehicleForces::FApplyForce_DATA(Force, bAccelChange));

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	//if (GVehicleMovementDebugParams.bShowAllForces)
	//{
	//	const FVector Position = RigidHandle->X();
	//	Chaos::FDebugDrawQueue::GetInstance().DrawDebugDirectionalArrow(Position, Position + Force * GVehicleMovementDebugParams.ForceDebugScaling, 20.0f, FColor::White, false, 0, SDPG_World, 2.0f);
	//}
#endif
}

void AAdvancedVehiclePawn::AddForceAtLocation(const FVector& Force, const FVector& Location, bool bIsLocalForce)
{
	if (!bIsLocalForce)
	{
		VehicleForce.Add(FAdvancedVehicleForces::FApplyForceAtLocation_DATA(Force, Location));

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		//if (GVehicleMovementDebugParams.bShowAllForces)
		//{
		//	Chaos::FDebugDrawQueue::GetInstance().DrawDebugDirectionalArrow(Position, Position + Force * GVehicleMovementDebugParams.ForceDebugScaling, 20.0f, FColor::White, false, 0, SDPG_World, 2.0f);
		//}
#endif
	}
	else
	{
		VehicleForce.Add(FAdvancedVehicleForces::FApplyForceAtLocationLocal_DATA(Force, Location));

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		//if (GVehicleMovementDebugParams.bShowAllForces)
		//{
		//	const FTransform WorldTM(RigidHandle->R(), RigidHandle->X());
		//	const FVector WorldPosition = WorldTM.TransformPosition(Position);
		//	const FVector WorldForce = WorldTM.TransformVector(Force);
		//	Chaos::FDebugDrawQueue::GetInstance().DrawDebugDirectionalArrow(WorldPosition, WorldPosition + WorldForce * GVehicleMovementDebugParams.ForceDebugScaling, 20.0f, FColor::White, false, 0, SDPG_World, 2.0f);
		//}
#endif
	}
}

void AAdvancedVehiclePawn::AddTorqueInRadians(const FVector& Torque, bool bAccelChange)
{
	VehicleForce.Add(FAdvancedVehicleForces::FAddTorqueInRadians_DATA(Torque, bAccelChange));
}

void AAdvancedVehiclePawn::AddTorqueAtLocation(const FVector& Torque, const FVector& Location, float ArmLength, bool bAccelChange)
{
	VehicleForce.Add(FAdvancedVehicleForces::FAddTorqueAtLocation_DATA(Torque, Location, ArmLength, bAccelChange));
}

void AAdvancedVehiclePawn::AddImpulse(const FVector& Impulse, bool bVelChange)
{
	VehicleForce.Add(FAdvancedVehicleForces::FAddImpulse_DATA(Impulse, bVelChange));
}

void AAdvancedVehiclePawn::AddImpulseAtLocation(const FVector& Impulse, const FVector& Location)
{
	VehicleForce.Add(FAdvancedVehicleForces::FAddImpulseAtLocation_DATA(Impulse, Location));
}

void AAdvancedVehiclePawn::AddAngularImpulseInRadians(const FVector& Impulse, bool bVelChange)
{
	VehicleForce.Add(FAdvancedVehicleForces::FAddAngularImpulseInRadians_DATA(Impulse, bVelChange));
}

FVector AAdvancedVehiclePawn::GetLinearVelocityAtPoint(const UPrimitiveComponent* TargetComponent, const FVector& InPoint)
{
	if (ensure(TargetComponent))
	{
		if (const FBodyInstance* BodyInstance = TargetComponent->GetBodyInstance())
		{
			return FAdvancedVehicleForces::GetVelocityAtPoint(BodyInstance, InPoint);
		}
	}

	return FVector::ZeroVector;
}

void AAdvancedVehiclePawn::SetThrottleInput(float InInput)
{
	VehicleInput.ThrottleInput = FMath::Clamp(InInput, 0.0f, 1.0f);
}

void AAdvancedVehiclePawn::SetSteeringInput(float InInput)
{
	VehicleInput.SteeringInput = FMath::Clamp(InInput, -1.0f, 1.0f);
}

void AAdvancedVehiclePawn::SetBrakingInput(float InInput)
{
	VehicleInput.BrakingInput = FMath::Clamp(InInput, 0.0f, 1.0f);
}

void AAdvancedVehiclePawn::SetHandbrakeInput(float InInput)
{
	VehicleInput.HandbrakeInput = FMath::Clamp(InInput, 0.0f, 1.0f);
}

void AAdvancedVehiclePawn::SetClutchInput(float InInput)
{
	VehicleInput.ClutchInput = FMath::Clamp(InInput, 0.0f, 1.0f);
}