// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedVehicleDynamics.generated.h"

USTRUCT(BlueprintType)
struct ADVANCEDVEHICLE_API FGarrettMap
{
	GENERATED_BODY()
public:
	FGarrettMap()
	{
		//RPMPoints.Reset();
		RPMPoints =
		{
			50000, 65000, 80000, 95000, 110000, 125000, 140000, 155000, 170000, 185000, 200000
		};

		//McorrPoints.Reset();
		McorrPoints =
		{
			0.04f, 0.06f, 0.08f, 0.10f, 0.12f, 0.14f, 0.16f, 0.18f, 0.20f, 0.22f, 0.24f, 0.26f, 0.28f, 0.30f
		};

		const int32 NR = RPMPoints.Num();
		const int32 NF = McorrPoints.Num();

		PRGrid.SetNum(NR * NF);
		EfficiencyGrid.SetNum(NR * NF);
		SurgeMcorrByRPM.SetNum(NR);
		ChokeMcorrByRPM.SetNum(NR);

		for (int32 i = 0; i < NR; ++i)
		{
			float t = (float)i / (NR - 1);

			SurgeMcorrByRPM[i] = FMath::Lerp(0.05f, 0.12f, t);
			ChokeMcorrByRPM[i] = FMath::Lerp(0.18f, 0.30f, t);
		}

		for (int32 r = 0; r < NR; ++r)
		{
			float RPMNorm = (float)r / (NR - 1);
			float PRMax = FMath::Lerp(1.3f, 3.0f, RPMNorm);

			for (int32 f = 0; f < NF; ++f)
			{
				float Flow = McorrPoints[f];

				float FlowMin = SurgeMcorrByRPM[r];
				float FlowMax = ChokeMcorrByRPM[r];
				float FlowNorm = FMath::Clamp((Flow - FlowMin) / (FlowMax - FlowMin), 0.f, 1.f);

				float FlowShape = 1.0f - FMath::Square(FlowNorm - 0.55f) * 3.0f;
				FlowShape = FMath::Clamp(FlowShape, 0.0f, 1.0f);

				float PR = 1.0f + (PRMax - 1.0f) * FlowShape;

				PRGrid[r * NF + f] = PR;

				float EffCenterFlow = 0.55f;
				float EffCenterRPM = 0.55f;

				float dF = FlowNorm - EffCenterFlow;
				float dR = RPMNorm - EffCenterRPM;

				float Dist2 = dF * dF + dR * dR;

				float Eff = 0.78f - Dist2 * 0.6f;
				Eff = FMath::Clamp(Eff, 0.55f, 0.80f);

				EfficiencyGrid[r * NF + f] = Eff;
			}
		}
	}

	/** Compressor RPM speed axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GarrettMap|Axes")
	TArray<float> RPMPoints;

	/** Corrected mass flow axis (kg/s) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GarrettMap|Axes")
	TArray<float> McorrPoints;

	/** Pressure ratio grid PR(Ncorr, Mcorr). Size must be RPMPoints.Num * McorrPoints.Num */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GarrettMap|Grids")
	TArray<float> PRGrid;

	/** Efficiency grid eta(Ncorr, Mcorr). Size must be RPMPoints.Num * McorrPoints.Num */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GarrettMap|Grids")
	TArray<float> EfficiencyGrid;

	/** Surge boundary: minimal Mcorr for each RPM row. Size must be RPMPoints.Num */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GarrettMap|Limits")
	TArray<float> SurgeMcorrByRPM;

	/** Choke / max-flow boundary: maximal Mcorr for each RPM row. Size must be RPMPoints.Num */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GarrettMap|Limits")
	TArray<float> ChokeMcorrByRPM;

	/** Default PR if map is invalid */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GarrettMap|Options")
	float DefaultPR = 1.0f;

	/** Default efficiency if map is invalid */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GarrettMap|Options")
	float DefaultEfficiency = 0.70f;

	float TrefK = 288.15f; // 15 C
	float PrefKPa = 101.325f;

public:
	bool IsValid() const
	{
		const int32 NR = RPMPoints.Num();
		const int32 NF = McorrPoints.Num();

		if (NR < 2 || NF < 2) return false;

		if (PRGrid.Num() != NR * NF) return false;
		if (EfficiencyGrid.Num() != NR * NF) return false;

		if (SurgeMcorrByRPM.Num() != NR) return false;
		if (ChokeMcorrByRPM.Num() != NR) return false;

		for (int32 i = 0; i < NR; ++i)
		{
			if (SurgeMcorrByRPM[i] >= ChokeMcorrByRPM[i])
			{
				return false;
			}
		}

		return true;
	}

	/** mCorr = m * sqrt(Tin/Tref) / (Pin/Pref) */
	float ComputeMcorr(float MdotKgPerSec, float TinK, float PinKPa) const
	{
		// Guard
		TinK = FMath::Max(TinK, 1.0f);
		PinKPa = FMath::Max(PinKPa, 0.001f);

		const float TempFactor = FMath::Sqrt(TinK / FMath::Max(TrefK, 1.0f));
		const float PressFactor = PinKPa / FMath::Max(PrefKPa, 0.001f);

		return MdotKgPerSec * TempFactor / PressFactor;
	}

	/** NCorr = N * sqrt(Tref/Tin) */
	float ComputeNcorr(float TurboRPM, float TinK) const
	{
		TinK = FMath::Max(TinK, 1.0f);
		return TurboRPM * FMath::Sqrt(FMath::Max(TrefK, 1.0f) / TinK);
	}

	/**
	 * Samples PR and efficiency for given (RPM, Mcorr).
	 * If inputs are not corrected and bInputsAreCorrected=false, you should pass TinK/PinKPa/Mdot for correction
	 * and call SampleFromPhysicalInputs().
	 */
	void SampleCorrected(float NcorrRPM, float McorrKgPerSec, float& OutPR, float& OutEff, bool bClampToMap = true) const
	{
		if (!IsValid())
		{
			OutPR = DefaultPR;
			OutEff = DefaultEfficiency;
			return;
		}

		NcorrRPM = FMath::Clamp(NcorrRPM, RPMPoints[0], RPMPoints.Last());

		if (bClampToMap)
		{
			McorrKgPerSec = ClampFlowByBoundaries(NcorrRPM, McorrKgPerSec);
		}

		OutPR = Sample2D(PRGrid, NcorrRPM, McorrKgPerSec, DefaultPR);
		OutEff = Sample2D(EfficiencyGrid, NcorrRPM, McorrKgPerSec, DefaultEfficiency);

		OutEff = FMath::Clamp(OutEff, 0.01f, 1.0f);
		OutPR = FMath::Max(OutPR, 1.0f); // PR < 1 isn't meaningful for a compressor map (usually)
	}

	/**
	 * Convenience: sample using physical inputs (non-corrected).
	 * - TurboRPM: actual turbo RPM
	 * - MdotKgPerSec: actual mass flow through compressor
	 * - TinK / PinKPa: compressor inlet conditions
	 */
	void SampleFromPhysicalInputs(
		float TurboRPM,
		float MdotKgPerSec,
		float TinK,
		float PinKPa,
		float& OutPR,
		float& OutEff,
		bool bClampToMap = true
	) const
	{
		/*if (bInputsAreCorrected)
		{
			SampleCorrected(TurboRPM, MdotKgPerSec, OutPR, OutEff, bClampToMap);
			return;
		}*/

		const float Ncorr = ComputeNcorr(TurboRPM, TinK);
		const float Mcorr = ComputeMcorr(MdotKgPerSec, TinK, PinKPa);

		SampleCorrected(Ncorr, Mcorr, OutPR, OutEff, bClampToMap);
	}

private:
	FORCEINLINE int32 GridIndex(int32 RpmIdx, int32 FlowIdx) const
	{
		return RpmIdx * McorrPoints.Num() + FlowIdx;
	}

	/** Finds segment index i such that Points[i] <= X <= Points[i+1], plus alpha in [0..1]. */
	static void FindSegment(const TArray<float>& Points, float X, int32& OutI, float& OutAlpha)
	{
		const int32 N = Points.Num();
		if (N < 2)
		{
			OutI = 0;
			OutAlpha = 0.0f;
			return;
		}

		// Clamp
		if (X <= Points[0])
		{
			OutI = 0;
			OutAlpha = 0.0f;
			return;
		}
		if (X >= Points[N - 1])
		{
			OutI = N - 2;
			OutAlpha = 1.0f;
			return;
		}

		int32 i = 0;
		for (; i < N - 1; ++i)
		{
			if (X <= Points[i + 1])
				break;
		}

		const float A = Points[i];
		const float B = Points[i + 1];
		const float Den = FMath::Max(B - A, KINDA_SMALL_NUMBER);

		OutI = i;
		OutAlpha = (X - A) / Den;
		OutAlpha = FMath::Clamp(OutAlpha, 0.0f, 1.0f);
	}

	float Sample2D(const TArray<float>& Grid, float NcorrRPM, float McorrKgPerSec, float DefaultValue) const
	{
		const int32 NR = RPMPoints.Num();
		const int32 NF = McorrPoints.Num();

		if (NR < 2 || NF < 2 || Grid.Num() != NR * NF)
		{
			return DefaultValue;
		}

		int32 iR = 0;
		float aR = 0.0f;
		FindSegment(RPMPoints, NcorrRPM, iR, aR);

		int32 iF = 0;
		float aF = 0.0f;
		FindSegment(McorrPoints, McorrKgPerSec, iF, aF);

		const float V00 = Grid[GridIndex(iR, iF)];
		const float V10 = Grid[GridIndex(iR + 1, iF)];
		const float V01 = Grid[GridIndex(iR, iF + 1)];
		const float V11 = Grid[GridIndex(iR + 1, iF + 1)];

		// Bilinear
		const float V0 = FMath::Lerp(V00, V01, aF);
		const float V1 = FMath::Lerp(V10, V11, aF);
		return FMath::Lerp(V0, V1, aR);
	}

	float ClampFlowByBoundaries(float NcorrRPM, float McorrKgPerSec) const
	{
		int32 iR = 0;
		float aR = 0.0f;
		FindSegment(RPMPoints, NcorrRPM, iR, aR);

		const float Surge0 = SurgeMcorrByRPM[iR];
		const float Surge1 = SurgeMcorrByRPM[iR + 1];
		const float Choke0 = ChokeMcorrByRPM[iR];
		const float Choke1 = ChokeMcorrByRPM[iR + 1];

		const float Surge = FMath::Lerp(Surge0, Surge1, aR);
		const float Choke = FMath::Lerp(Choke0, Choke1, aR);

		const float MinFlow = Surge + 1e-4f;
		const float MaxFlow = Choke - 1e-4f;

		return FMath::Clamp(McorrKgPerSec, MinFlow, MaxFlow);
	}
};

USTRUCT(BlueprintType)
struct ADVANCEDVEHICLE_API FVETable
{
	GENERATED_BODY()
public:
	FVETable()
	{
		RPMPoints.Reset();
		RPMPoints =
		{
			800,
			1500,
			2500,
			3500,
			4500,
			5500,
			6500,
			7500,
			8500,
			9500
		};

		MAPPoints.Reset();
		MAPPoints =
		{
			20,
			40,
			60,
			100,
			140,
			200,
			220
		};

		VEGrid.Reset();
		VEGrid =
		{
			// MAP = 20 kPa (more vacuum)
			25, 27, 28, 28, 27, 26, 25, 24, 23, 22,
			// MAP = 40 kPa
			32, 38, 45, 50, 52, 51, 48, 45, 42, 38,
			// MAP = 60 kPa
			48, 58, 70, 78, 83, 85, 80, 75, 68, 60,
			// MAP = 100 kPa (atmospheric maximum)
			62, 75, 88, 95, 100, 102, 98, 92, 85, 76,
			// MAP = 140 kPa (lite turbo / ram-air resonance)
			68, 82, 94, 100, 104, 106, 102, 96, 88, 80,
			// MAP = 200 kPa (mild turbo / resonance placement)
			70, 85, 96, 102, 106, 108, 105, 100, 92, 84,
			// MAP = 220 kPa (high turbo)
			65, 86, 100, 105, 110, 108, 95, 92, 75, 70
		};
	}

	/** RPM VE-axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> RPMPoints;

	/** MAP VE-axis (kPa) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> MAPPoints;

	/** 2D VE: MAP × RPM */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> VEGrid;

	/** Returns VE ratio from table and current values (RPM, MAP) */
	float GetVE(float InRPM, float InMAP) const;
};