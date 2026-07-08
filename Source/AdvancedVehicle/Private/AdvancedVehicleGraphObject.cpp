// Fill out your copyright notice in the Description page of Project Settings.

#include "AdvancedVehicleGraphObject.h"

UAdvancedVehicleGraphObject::UAdvancedVehicleGraphObject()
{
    UpdateCurves();
}

void UAdvancedVehicleGraphObject::UpdateCurves()
{
    TireGraphCurve.EditorCurveData.Reset();
    {
        const float Exp = BeforePeakExponent;
        const float Amount = AfterPeakAmount;
        const float Stiff = AfterPeakStiffness;

        for (int i = 0; i <= 100; ++i)
        {
            const float X = i / 10.0f;

            const float Before = 1 - FMath::Pow(1 - X, Exp);
            const float Dx = X - 1.0f;
            const float After = 1.0f + Amount * 0.63f * FMath::Atan(Stiff * Dx * Dx) * 2.0f;

            const float Value = X <= 1.0f ? Before : After;
            TireGraphCurve.EditorCurveData.AddKey(X, Value);

            if (IsMirrorSide)
            {
                if (i > 0)
                {
                    TireGraphCurve.EditorCurveData.AddKey(-X, -Value);
                }
            }
        }
    }

    CompressorMapCurve.EditorCurveData.Reset();
    {
        FRichCurve& Curve = CompressorMapCurve.EditorCurveData;
        Curve.Reset();

        const float MinX = CompressorMap.McorrPoints.Num() ? CompressorMap.McorrPoints[0] : 0.04f;
        const float MaxX = CompressorMap.McorrPoints.Num() ? CompressorMap.McorrPoints.Last() : 0.30f;

        const int32 N = FMath::Max(32, 2);

        for (int32 i = 0; i < N; ++i)
        {
            const float A = (float)i / (N - 1);
            const float Mc = FMath::Lerp(MinX, MaxX, A);

            float PR = 1.0f, Eff = 0.7f;
            CompressorMap.SampleCorrected(PreviewRPM, Mc, PR, Eff, /*bClampToMap=*/true);

            FKeyHandle H = Curve.AddKey(Mc, PR);
            Curve.SetKeyInterpMode(H, ERichCurveInterpMode::RCIM_Linear);
        }

        //for (int i = 0; i <= 100; ++i)
        {
            




            //float ShaftRPMc = CorrectedSpeedRPM(Charger.RPM, IntakeTempK);
            //float Flowc = CorrectedMassFlow(EngineMassFlow, IntakeTempK, IntakePressurePa);

            //float PR = 1.0f, Eff = 0.6f;
            //CompressorMap.Sample(Flowc, ShaftRPMc, PR, Eff);

            //// Дальше PR -> прирост давления.
            //// Например:
            //const float Pout = IntakePressurePa * PR;

            //// Температура после компрессора (упрощенно, через изоэнтропическую эффективность):
            //// Tout = Tin * [ 1 + (PR^((k-1)/k) - 1)/Eff ]
            //const float k = 1.4f;
            //const float exp = (k - 1.0f) / k;
            //const float ToutK = IntakeTempK * (1.0f + (FMath::Pow(PR, exp) - 1.0f) / FMath::Max(Eff, 0.01f));

            //LUTMapCurve.EditorCurveData.AddKey(mcorr, PR);
        }
    }

	VE = VETable.GetVE(CurrentRPM, CurrentIMAP);
}

void UAdvancedVehicleGraphObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateCurves();
}