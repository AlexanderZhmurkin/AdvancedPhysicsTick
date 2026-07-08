// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Canvas.h"
#include "GameFramework/HUD.h"
#include "SimpleVehicleHUD.generated.h"

USTRUCT(BlueprintType)
struct ADVANCEDVEHICLE_API FDebugGraphPoint
{
	GENERATED_BODY()
public:
	FDebugGraphPoint()
	{
		Title = "";
		Value = FVector2D::ZeroVector;
		Color = FLinearColor::Yellow;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color;
};

UCLASS(Config = Game)
class ADVANCEDVEHICLE_API ASimpleVehicleHUD : public AHUD
{
	GENERATED_UCLASS_BODY()
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** For draw 2D telemetry from OnHUDPostRender */
	virtual void OnHUDPostRender(AHUD* InHUD, UCanvas* InCanvas);
	/** Store 2D debug information */
	virtual void DrawDebug2D(UCanvas* InCanvas, float& YL, float& YPos);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD", meta = (DisplayName = "Canvas Draw Debug 2D"))
	void BlueprintDrawDebug2D(UCanvas* InCanvas, float YL, float YPos);

public:
	UFUNCTION(BlueprintCallable, Category = "Game|Debug|SimpleVehicleHUD")
	void DrawDebugGraph(
		UCanvas* InCanvas, 
		const FString& Title, 
		float ValueX, 
		float ValueY, 
		float UL_X, 
		float UL_Y, 
		float W, 
		float H, 
		FVector2D RangeX, 
		FVector2D RangeY);

	UFUNCTION(BlueprintCallable, Category = "Game|Debug|SimpleVehicleHUD")
	void DrawDebugAdvancedGraph(
		UCanvas* InCanvas, 
		const FString& Title, 
		const TArray<FDebugGraphPoint>& Points,
		float UL_X,
		float UL_Y,
		float W,
		float H,
		FVector2D RangeX,
		FVector2D RangeY,
		const FString& TitleX = "X",
		const FString& TitleY = "Y",
		int32 GridX = 0,
		int32 GridY = 0);

	UFUNCTION(BlueprintCallable, Category = "Game|Debug|SimpleVehicleHUD")
	void DrawDebugCircle(UCanvas* InCanvas, FVector2D Center, FVector2D StartPos, FLinearColor Color = FLinearColor::White, int32 Segments = 16, float CircleRadius = 32.0f, float LineThickness = 1.0f);
};