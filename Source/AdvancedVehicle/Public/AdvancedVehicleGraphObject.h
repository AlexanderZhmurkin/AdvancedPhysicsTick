// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Curves/CurveFloat.h"
#include "AdvancedVehicleDynamics.h"
#include "AdvancedVehicleUtility.h"
#include "AdvancedVehicleGraphObject.generated.h"

USTRUCT(BlueprintType)
struct FGraphLine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Graph")
	FName Name = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Graph")
	TArray<FVector2D> Points;

	UPROPERTY(EditAnywhere, Category = "Graph")
	FLinearColor Color = FLinearColor::Green;

	UPROPERTY(EditAnywhere, Category = "Graph", meta = (ClampMin = "1.0", ClampMax = "6.0"))
	float Thickness = 2.f;

	UPROPERTY(EditAnywhere, Category = "Graph")
	bool bVisible = true;
};

UCLASS(Blueprintable, BlueprintType)
class ADVANCEDVEHICLE_API UAdvancedVehicleGraphObject : public UObject
{
	GENERATED_BODY()
public:
	UAdvancedVehicleGraphObject();

	void UpdateCurves();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	UPROPERTY(EditAnywhere, Category = "Graph")
	TArray<FGraphLine> Lines;

	UPROPERTY(EditAnywhere, Category = "Graph")
	bool bUseManualBounds = false;

	UPROPERTY(EditAnywhere, Category = "Graph", meta = (EditCondition = "bUseManualBounds"))
	FVector2D ManualMin = FVector2D(0, 0);

	UPROPERTY(EditAnywhere, Category = "Graph", meta = (EditCondition = "bUseManualBounds"))
	FVector2D ManualMax = FVector2D(1, 1);

protected:
	UPROPERTY(VisibleAnywhere, Category = "TireGraph", DisplayName = "Tire Preview")
	FRuntimeFloatCurve TireGraphCurve;

	UPROPERTY(EditAnywhere, Category = "TireGraph")
	bool IsMirrorSide = true;

	UPROPERTY(EditAnywhere, Category = "TireGraph")
	float BeforePeakExponent = 2.0f;

	UPROPERTY(EditAnywhere, Category = "TireGraph")
	float AfterPeakAmount = 0.1f;

	UPROPERTY(EditAnywhere, Category = "TireGraph")
	float AfterPeakStiffness = -1.2f;

	UPROPERTY(VisibleAnywhere, Category = "Engine|CompressorMap", DisplayName = "Compressor Map Preview")
	FRuntimeFloatCurve CompressorMapCurve;

	UPROPERTY(VisibleAnywhere, Category = "Engine|BoostMap", DisplayName = "Boost LUT Preview")
	FRuntimeFloatCurve BoostLUTCurve;

	UPROPERTY(EditAnywhere, Category = "Engine|Compressor")
	FGarrettMap CompressorMap;

	UPROPERTY(EditAnywhere, Category = "Engine|Compressor")
	float PreviewRPM = 140000.0f;

	UPROPERTY(EditAnywhere, Category = "Engine|Compressor")
	float PreviewMcorr = 0.16f;

	UPROPERTY(EditAnywhere, Category = "Engine|VETable")
	FVETable VETable;

	UPROPERTY(EditAnywhere, Category = "Engine|VETable")
	float CurrentRPM = 800.0f;

	UPROPERTY(EditAnywhere, Category = "Engine|VETable")
	float CurrentIMAP = 100.0f;

	UPROPERTY(VisibleAnywhere, Category = "Engine|VETable")
	float VE;
};