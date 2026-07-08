// Fill out your copyright notice in the Description page of Project Settings.

#include "AdvancedVehicleDynamics.h"

float FVETable::GetVE(float InRPM, float InMAP) const
{
	const int32 NumRPM = RPMPoints.Num();
	const int32 NumMAP = MAPPoints.Num();
	const int32 NumCellsPerRow = NumRPM;

	if (NumRPM == 0 || NumMAP == 0 || VEGrid.Num() == 0)
	{
		return 0.0f;
	}

	int32 R0 = 0;
	for (int32 i = 0; i < NumRPM - 1; i++)
	{
		if (InRPM >= RPMPoints[i] && InRPM <= RPMPoints[i + 1])
		{
			R0 = i;
			break;
		}

		if (InRPM < RPMPoints[0])
		{
			R0 = 0;
			break;
		}

		if (InRPM > RPMPoints[NumRPM - 1])
		{
			R0 = NumRPM - 2;
			break;
		}
	}

	int32 R1 = R0 + 1;

	const float RPM0 = RPMPoints[R0];
	const float RPM1 = RPMPoints[R1];
	const float TRPM = (RPM1 - RPM0) > 0 ? (InRPM - RPM0) / (RPM1 - RPM0) : 0.0f;

	int32 M0 = 0;
	for (int32 j = 0; j < NumMAP - 1; j++)
	{
		if (InMAP >= MAPPoints[j] && InMAP <= MAPPoints[j + 1])
		{
			M0 = j;
			break;
		}

		if (InMAP < MAPPoints[0])
		{
			M0 = 0;
			break;
		}

		if (InMAP > MAPPoints[NumMAP - 1])
		{
			M0 = NumMAP - 2;
			break;
		}
	}

	const int32 M1 = M0 + 1;

	const float MAP0 = MAPPoints[M0];
	const float MAP1 = MAPPoints[M1];
	const float TBMAP = (MAP1 - MAP0) > 0 ? (InMAP - MAP0) / (MAP1 - MAP0) : 0.0f;

	const float VE00 = VEGrid[M0 * NumCellsPerRow + R0];
	const float VE10 = VEGrid[M0 * NumCellsPerRow + R1];
	const float VE01 = VEGrid[M1 * NumCellsPerRow + R0];
	const float VE11 = VEGrid[M1 * NumCellsPerRow + R1];

	const float VE0 = FMath::Lerp(VE00, VE10, TRPM);
	const float VE1 = FMath::Lerp(VE01, VE11, TRPM);
	const float VE = FMath::Lerp(VE0, VE1, TBMAP);

	return VE;
}