// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedVehicleUtility.generated.h"

USTRUCT(Blueprintable)
struct ADVANCEDVEHICLE_API FAdvancedVehicleInput
{
	GENERATED_USTRUCT_BODY()
public:
	FAdvancedVehicleInput()
		: ThrottleInput(0.0f)
		, SteeringInput(0.0f)
		, BrakingInput(0.0f)
		, HandbrakeInput(0.0f)
		, ClutchInput(0.0f)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ThrottleInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SteeringInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BrakingInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HandbrakeInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ClutchInput;
};

USTRUCT(Blueprintable)
struct ADVANCEDVEHICLE_API FAdvancedVehicleOutput
{
	GENERATED_USTRUCT_BODY()
public:
	FAdvancedVehicleOutput()
		: PhysicsDeltaTime(0.0f)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PhysicsDeltaTime;
};

USTRUCT(BlueprintType)
struct FFloatLUT2D
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LUT")
	TArray<float> XPoints; // например Corrected Mass Flow

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LUT")
	TArray<float> YPoints; // например Shaft RPM (или PressureRatio)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LUT")
	TArray<float> Grid;    // Ny * Nx

	bool IsValid() const
	{
		const int32 Nx = XPoints.Num();
		const int32 Ny = YPoints.Num();
		return Nx >= 2 && Ny >= 2 && Grid.Num() == Nx * Ny;
	}

private:
	static int32 FindSegment(const TArray<float>& Points, float V)
	{
		const int32 N = Points.Num();
		if (N < 2) return 0;

		if (V <= Points[0]) return 0;
		if (V >= Points[N - 1]) return N - 2;

		for (int32 i = 0; i < N - 1; ++i)
		{
			if (V >= Points[i] && V <= Points[i + 1])
			{
				return i;
			}
		}

		return N - 2;
	}

	float SampleIndex(int32 Xi, int32 Yi) const
	{
		const int32 Nx = XPoints.Num();
		return Grid[Yi * Nx + Xi];
	}

public:
	float Sample(float X, float Y) const
	{
		if (!IsValid())
		{
			return 0.0f;
		}

		const int32 Nx = XPoints.Num();
		const int32 Ny = YPoints.Num();

		const int32 x0 = FindSegment(XPoints, X);
		const int32 y0 = FindSegment(YPoints, Y);

		const int32 x1 = FMath::Min(x0 + 1, Nx - 1);
		const int32 y1 = FMath::Min(y0 + 1, Ny - 1);

		const float X0 = XPoints[x0];
		const float X1 = XPoints[x1];
		const float Y0 = YPoints[y0];
		const float Y1 = YPoints[y1];

		const float tx = (FMath::IsNearlyEqual(X0, X1)) ? 0.0f : FMath::Clamp((X - X0) / (X1 - X0), 0.0f, 1.0f);
		const float ty = (FMath::IsNearlyEqual(Y0, Y1)) ? 0.0f : FMath::Clamp((Y - Y0) / (Y1 - Y0), 0.0f, 1.0f);

		const float v00 = SampleIndex(x0, y0);
		const float v10 = SampleIndex(x1, y0);
		const float v01 = SampleIndex(x0, y1);
		const float v11 = SampleIndex(x1, y1);

		const float vx0 = FMath::Lerp(v00, v10, tx);
		const float vx1 = FMath::Lerp(v01, v11, tx);
		return FMath::Lerp(vx0, vx1, ty);
	}
};

USTRUCT(BlueprintType)
struct FCompressorMap
{
	GENERATED_BODY()

public:
	// Оси (пример): X = corrected flow (kg/s), Y = shaft RPM
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compressor")
	FFloatLUT2D PressureRatioLUT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compressor")
	FFloatLUT2D EfficiencyLUT;

	// Опционально: ограничения
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compressor")
	float MinEfficiency = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compressor")
	float MaxEfficiency = 0.82f;

	// Возвращает (PR, Eff) по входам
	void Sample(float CorrectedFlow, float ShaftRPM, float& OutPR, float& OutEff) const
	{
		OutPR = PressureRatioLUT.IsValid() ? PressureRatioLUT.Sample(CorrectedFlow, ShaftRPM) : 1.0f;
		OutEff = EfficiencyLUT.IsValid() ? EfficiencyLUT.Sample(CorrectedFlow, ShaftRPM) : 0.6f;

		OutPR = FMath::Max(OutPR, 1.0f);
		OutEff = FMath::Clamp(OutEff, MinEfficiency, MaxEfficiency);
	}

	FCompressorMap MakeDemoMap() const
	{
		FCompressorMap M;

		// X: corrected flow (kg/s)
		M.PressureRatioLUT.XPoints = { 0.05f, 0.08f, 0.11f, 0.14f, 0.17f, 0.20f };
		M.EfficiencyLUT.XPoints = M.PressureRatioLUT.XPoints;

		// Y: shaft RPM
		M.PressureRatioLUT.YPoints = { 60000, 90000, 120000, 150000, 180000 };
		M.EfficiencyLUT.YPoints = M.PressureRatioLUT.YPoints;

		// Grid layout: rows are Y (RPM), columns are X (Flow)
		// Pressure Ratio values:
		M.PressureRatioLUT.Grid =
		{
			// 60k
			1.05f, 1.10f, 1.12f, 1.10f, 1.06f, 1.02f,
			// 90k
			1.15f, 1.25f, 1.32f, 1.30f, 1.22f, 1.10f,
			// 120k
			1.25f, 1.45f, 1.62f, 1.60f, 1.45f, 1.25f,
			// 150k
			1.35f, 1.65f, 1.90f, 1.88f, 1.70f, 1.45f,
			// 180k
			1.40f, 1.75f, 2.05f, 2.02f, 1.85f, 1.55f
		};

		// Efficiency values (0..1):
		M.EfficiencyLUT.Grid =
		{
			// 60k
			0.45f, 0.52f, 0.55f, 0.53f, 0.48f, 0.42f,
			// 90k
			0.50f, 0.62f, 0.68f, 0.66f, 0.58f, 0.48f,
			// 120k
			0.52f, 0.68f, 0.75f, 0.74f, 0.66f, 0.54f,
			// 150k
			0.50f, 0.66f, 0.78f, 0.77f, 0.70f, 0.58f,
			// 180k
			0.46f, 0.60f, 0.72f, 0.71f, 0.64f, 0.54f
		};

		return M;
	}
};





FORCEINLINE float CorrectedMassFlow(float W, float TinK, float PinPa)
{
	const float Tref = 288.15f;
	const float Pref = 101325.0f;
	return W * FMath::Sqrt(TinK / Tref) / (PinPa / Pref);
}

FORCEINLINE float CorrectedSpeedRPM(float ShaftRPM, float TinK)
{
	const float Tref = 288.15f;
	return ShaftRPM / FMath::Sqrt(TinK / Tref);
}



USTRUCT(BlueprintType)
struct ADVANCEDVEHICLE_API FTurboMapTable
{
	GENERATED_BODY()
public:
	FTurboMapTable()
	{
		TurboRPMPoints.Reset();
		TurboRPMPoints =
		{
			50000, 60000, 70000, 80000, 90000, 100000,
			110000, 120000, 130000, 140000, 150000, 
			160000, 170000, 180000, 190000, 200000, 220000
		};

		McorrPoints.Reset();
		McorrPoints =
		{
			10, 15, 20, 25, 30, 35
		};














		const int32 NumRPM = TurboRPMPoints.Num();
		const int32 NumM = McorrPoints.Num();

		PRGrid.SetNumZeroed(NumRPM * NumM);
		EffGrid.SetNumZeroed(NumRPM * NumM);

		// Пример генерации “похожей формы”, чтобы таблица не была пустой:
		for (int32 m = 0; m < NumM; ++m)
		{
			const float Flow = McorrPoints[m];
			const float FlowN = (Flow - McorrPoints[0]) / (McorrPoints.Last() - McorrPoints[0]); // 0..1

			for (int32 r = 0; r < NumRPM; ++r)
			{
				const float RPM = TurboRPMPoints[r];
				const float RpmN = (RPM - TurboRPMPoints[0]) / (TurboRPMPoints.Last() - TurboRPMPoints[0]); // 0..1

				// PR: выше на больших RPM, падает на больших расходах
				float PR = 1.0f + (1.0f + 2.6f * RpmN) * (1.0f - 0.9f * FlowN - 0.25f * FlowN * FlowN);
				PR = FMath::Max(PR, 1.02f);

				// Eff: “остров” в середине
				const float x = (FlowN - 0.55f);
				const float y = (RpmN - 0.55f);
				float Eff = 0.78f - 0.55f * (x * x + 0.9f * y * y);
				Eff = FMath::Clamp(Eff, 0.35f, 0.82f);

				PRGrid[m * NumRPM + r] = PR;
				EffGrid[m * NumRPM + r] = Eff;
			}
		}
	}

	/** RPM axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> TurboRPMPoints;

	/** Mcorr axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> McorrPoints;

	/** Pressure Ratio 2D: mcorr × rpm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> PRGrid;

	/** Effective 2D: mcorr × rpm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> EffGrid;

	float SampleGrid(const TArray<float>& Grid, float InTurboRPM, float InMcorr, float DefaultValue) const;

	void Sample(float InTurboRPM, float InMcorr, float& OutPR, float& OutEff) const
	{
		OutPR = SampleGrid(PRGrid, InTurboRPM, InMcorr, 1.0f);
		OutEff = SampleGrid(EffGrid, InTurboRPM, InMcorr, 0.6f);
		OutPR = FMath::Max(OutPR, 1.0f);
		OutEff = FMath::Clamp(OutEff, 0.01f, 1.0f);
	}
};

USTRUCT(BlueprintType)
struct FCompressorSpeedLine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float RPM = 100000.0f;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve PR_vs_Mcorr; // X=mcorr (kg/s), Y=PR

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve Eff_vs_Mcorr; // X=mcorr (kg/s), Y=eff (0..1)
};



struct ADVANCEDVEHICLE_API FAdvancedVehicleStatus
{
public:
	FAdvancedVehicleStatus()
		: VehicleWorldTransform(FTransform::Identity)
		, VehicleWorldVelocity(FVector::ZeroVector)
		, VehicleWorldVelocityNormal(FVector::ZeroVector)
		, VehicleWorldAngularVelocity(FVector::ZeroVector)
		, VehicleWorldCOM(FVector::ZeroVector)
		, VehicleXVector(FVector::ZeroVector)
		, VehicleYVector(FVector::ZeroVector)
		, VehicleZVector(FVector::ZeroVector)
		, VehicleForwardSpeed(0.0f)
		, VehicleRightSpeed(0.0f)
		, VehicleDriftAngle(0.0f)
		, VehicleDriftSide(0.0f)
		, VehicleLocalVelocity(FVector::ZeroVector)
		, VehicleLocalAcceleration(FVector::ZeroVector)
		, VehicleLocalGForce(FVector::ZeroVector)
		, VehicleSleeping(false)
		, VehicleSleepingCounter(0)
		, LastVehicleLocalVelocity(FVector::ZeroVector)
	{
	}

	/** Particle Handle cache capture state */
	void CaptureState(float DeltaTime, float GravityZ, const Chaos::FRigidBodyHandle_Internal* TargetHandle);

	FTransform VehicleWorldTransform;
	FVector VehicleWorldVelocity;
	FVector VehicleWorldVelocityNormal;
	FVector VehicleWorldAngularVelocity;
	FVector VehicleWorldCOM;

	FVector VehicleXVector;   //X Vector
	FVector VehicleYVector;   //Y Vector
	FVector VehicleZVector;   //Z Vector

	float VehicleForwardSpeed;
	float VehicleRightSpeed;

	float VehicleDriftAngle;
	float VehicleDriftSide;

	FVector VehicleLocalVelocity;
	FVector VehicleLocalAcceleration;
	FVector VehicleLocalGForce;

	bool VehicleSleeping;
	int VehicleSleepingCounter;

private:
	FVector LastVehicleLocalVelocity;
};

class ADVANCEDVEHICLE_API FAdvancedVehicleForces
{
public:
	struct FApplyForce_DATA
	{
		FApplyForce_DATA(const FVector& InForce, bool bInAccelChange)
			: Force(InForce), bAccelChange(bInAccelChange)
		{
		}

		FVector Force;
		bool bAccelChange;
	};

	struct FApplyForceAtLocation_DATA
	{
		FApplyForceAtLocation_DATA(const FVector& InForce, const FVector& InLocation)
			: Force(InForce), Location(InLocation)
		{
		}

		FVector Force;
		FVector Location;
	};

	struct FApplyForceAtLocationLocal_DATA
	{
		FApplyForceAtLocationLocal_DATA(const FVector& InForce, const FVector& InLocation)
			: Force(InForce), Location(InLocation)
		{
		}

		FVector Force;
		FVector Location;
	};

	struct FAddTorqueInRadians_DATA
	{
		FAddTorqueInRadians_DATA(const FVector& InTorque, bool InbAccelChange)
			: Torque(InTorque), bAccelChange(InbAccelChange)
		{
		}

		FVector Torque;
		bool bAccelChange;
	};

	struct FAddTorqueAtLocation_DATA
	{
		FAddTorqueAtLocation_DATA(const FVector& InTorque, const FVector& InLocation, const float InLength, bool InbAccelChange)
			: Torque(InTorque), Location(InLocation), ArmLength(InLength), bAccelChange(InbAccelChange)
		{
		}

		FVector Torque;
		FVector Location;
		float ArmLength;
		bool bAccelChange;
	};

	struct FAddImpulse_DATA
	{
		FAddImpulse_DATA(const FVector& InImpulse, const bool InbVelChange)
			: Impulse(InImpulse), bVelChange(InbVelChange)
		{
		}

		FVector Impulse;
		bool bVelChange;
	};

	struct FAddImpulseAtLocation_DATA
	{
		FAddImpulseAtLocation_DATA(const FVector& InImpulse, const FVector& InLocation)
			: Impulse(InImpulse), Location(InLocation)
		{
		}

		FVector Impulse;
		FVector Location;
	};

	struct FAddAngularImpulseInRadians_DATA
	{
		FAddAngularImpulseInRadians_DATA(const FVector& InAngularImpulse, const bool InbVelChange)
			: Impulse(InAngularImpulse), bVelChange(InbVelChange)
		{
		}

		FVector Impulse;
		bool bVelChange;
	};

	void Add(const FApplyForce_DATA& InForceData)
	{
		ApplyForceList.Add(InForceData);
	}

	void Add(const FApplyForceAtLocation_DATA& InForceAtLocationData)
	{
		ApplyForceAtLocationList.Add(InForceAtLocationData);
	}

	void Add(const FApplyForceAtLocationLocal_DATA& InForceAtLocationLocalData)
	{
		ApplyForceAtLocationLocalList.Add(InForceAtLocationLocalData);
	}

	void Add(const FAddTorqueInRadians_DATA& InApplyTorqueData)
	{
		ApplyTorqueList.Add(InApplyTorqueData);
	}

	void Add(const FAddTorqueAtLocation_DATA& InApplyTorqueData)
	{
		ApplyTorqueAtLocationList.Add(InApplyTorqueData);
	}

	void Add(const FAddImpulse_DATA& InApplyImpulseData)
	{
		ApplyImpulseList.Add(InApplyImpulseData);
	}

	void Add(const FAddImpulseAtLocation_DATA& InApplyImpulseAtLocationData)
	{
		ApplyImpulseAtLocationList.Add(InApplyImpulseAtLocationData);
	}

	void Add(const FAddAngularImpulseInRadians_DATA& InAngularImpulseData)
	{
		ApplyAngularImpulseList.Add(InAngularImpulseData);
	}

	void Apply(Chaos::FRigidBodyHandle_Internal* RigidHandle)
	{
		for (const FApplyForce_DATA& Data : ApplyForceList)
		{
			AddForce(RigidHandle, Data);
		}

		for (const FApplyForceAtLocation_DATA& Data : ApplyForceAtLocationList)
		{
			AddForceAtLocation(RigidHandle, Data);
		}

		for (const FApplyForceAtLocationLocal_DATA& Data : ApplyForceAtLocationLocalList)
		{
			AddForceAtLocationLocal(RigidHandle, Data);
		}

		for (const FAddTorqueInRadians_DATA& Data : ApplyTorqueList)
		{
			AddTorque(RigidHandle, Data);
		}

		for (const FAddTorqueAtLocation_DATA& Data : ApplyTorqueAtLocationList)
		{
			AddTorqueAtLocation(RigidHandle, Data);
		}

		for (const FAddImpulse_DATA& Data : ApplyImpulseList)
		{
			AddImpulse(RigidHandle, Data);
		}

		for (const FAddImpulseAtLocation_DATA& Data : ApplyImpulseAtLocationList)
		{
			AddImpulseAtLocation(RigidHandle, Data);
		}

		for (const FAddAngularImpulseInRadians_DATA& Data : ApplyAngularImpulseList)
		{
			AddAngularImpulse(RigidHandle, Data);
		}

		ApplyForceList.Empty();
		ApplyForceAtLocationList.Empty();
		ApplyForceAtLocationLocalList.Empty();
		ApplyTorqueList.Empty();
		ApplyTorqueAtLocationList.Empty();
		ApplyImpulseList.Empty();
		ApplyImpulseAtLocationList.Empty();
		ApplyAngularImpulseList.Empty();
	}

	/** Returns the internal Chaos rigid body handle for the given primitive component and optional bone. */
	static Chaos::FRigidBodyHandle_Internal* GetInternalHandle(UPrimitiveComponent* InComponent, FName BoneName = NAME_None);
	/** Returns the velocity at a specific point on the body instance. */
	static FVector GetVelocityAtPoint(const FBodyInstance* InBodyInst, const FVector& InPoint, const bool bIsGT = false);
	/** Returns the velocity at a specific point on the Chaos rigid body handle. */
	static FVector GetVelocityAtPoint(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FVector& InPoint);

private:
	void AddForce(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FApplyForce_DATA& InData);
	void AddForceAtLocation(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FApplyForceAtLocation_DATA& InData);
	void AddForceAtLocationLocal(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FApplyForceAtLocationLocal_DATA& InData);
	void AddTorque(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FAddTorqueInRadians_DATA& InData);
	void AddTorqueAtLocation(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FAddTorqueAtLocation_DATA& InData);
	void AddImpulse(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FAddImpulse_DATA& InData);
	void AddImpulseAtLocation(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FAddImpulseAtLocation_DATA& InData);
	void AddAngularImpulse(Chaos::FRigidBodyHandle_Internal* InRigidHandle, const FAddAngularImpulseInRadians_DATA& InData);

	TArray<FApplyForce_DATA> ApplyForceList;
	TArray<FApplyForceAtLocation_DATA> ApplyForceAtLocationList;
	TArray<FApplyForceAtLocationLocal_DATA> ApplyForceAtLocationLocalList;
	TArray<FAddTorqueInRadians_DATA> ApplyTorqueList;
	TArray<FAddTorqueAtLocation_DATA> ApplyTorqueAtLocationList;
	TArray<FAddImpulse_DATA> ApplyImpulseList;
	TArray<FAddImpulseAtLocation_DATA> ApplyImpulseAtLocationList;
	TArray<FAddAngularImpulseInRadians_DATA> ApplyAngularImpulseList;
};