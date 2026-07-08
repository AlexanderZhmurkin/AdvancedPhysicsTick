// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AdvancedVehicleGraphObject.h"

class SGraph2DWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGraph2DWidget) {}
		SLATE_ARGUMENT(TWeakObjectPtr<UAdvancedVehicleGraphObject>, GraphObject)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		GraphObject = InArgs._GraphObject;
	}

	virtual int32 OnPaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override
	{
		const FVector2D Size = AllottedGeometry.GetLocalSize();
		const float Pad = 30.f;

		// Фон
		{
			const FLinearColor Bg = FLinearColor(0.03f, 0.03f, 0.03f, 1.f);
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId++,
				AllottedGeometry.ToPaintGeometry(),
				FCoreStyle::Get().GetBrush("WhiteBrush"),
				ESlateDrawEffect::None,
				Bg
			);
		}

		TWeakObjectPtr<UAdvancedVehicleGraphObject> Obj = GraphObject;
		if (!Obj.IsValid())
			return LayerId;

		const UAdvancedVehicleGraphObject* GO = Obj.Get();

		FVector2D Min(FLT_MAX, FLT_MAX);
		FVector2D Max(-FLT_MAX, -FLT_MAX);

		auto ExpandBounds = [&](const FVector2D& P)
		{
			Min.X = FMath::Min(Min.X, P.X); Min.Y = FMath::Min(Min.Y, P.Y);
			Max.X = FMath::Max(Max.X, P.X); Max.Y = FMath::Max(Max.Y, P.Y);
		};

		if (GO->bUseManualBounds)
		{
			Min = GO->ManualMin;
			Max = GO->ManualMax;
		}
		else
		{
			for (const FGraphLine& L : GO->Lines)
			{
				if (!L.bVisible) continue;
				for (const FVector2D& P : L.Points) ExpandBounds(P);
			}

			if (!FMath::IsFinite(Min.X) || !FMath::IsFinite(Max.X) || Min.X >= Max.X || Min.Y >= Max.Y)
			{
				Min = FVector2D(0, 0);
				Max = FVector2D(1, 1);
			}
		}

		// Рабочая область
		const FVector2D PlotMin(Pad, Pad);
		const FVector2D PlotMax(Size.X - Pad, Size.Y - Pad);
		const FVector2D PlotSize = PlotMax - PlotMin;

		const FVector2D Range = (Max - Min);
		const FVector2D SafeRange(
			FMath::Max(Range.X, KINDA_SMALL_NUMBER),
			FMath::Max(Range.Y, KINDA_SMALL_NUMBER)
		);

		auto ToScreen = [&](const FVector2D& P) -> FVector2D
		{
			const float NX = (P.X - Min.X) / SafeRange.X;
			const float NY = (P.Y - Min.Y) / SafeRange.Y;

			const float SX = PlotMin.X + NX * PlotSize.X;
			const float SY = PlotMax.Y - NY * PlotSize.Y;
			return FVector2D(SX, SY);
		};

		{
			const int32 Divs = 10;
			TArray<FVector2D> LinePts;
			const FLinearColor Grid = FLinearColor(1,1,1,0.06f);

			for (int32 i = 1; i < Divs; ++i)
			{
				const float T = (float)i / (float)Divs;

				LinePts.Reset();
				LinePts.Add(FVector2D(PlotMin.X + T * PlotSize.X, PlotMin.Y));
				LinePts.Add(FVector2D(PlotMin.X + T * PlotSize.X, PlotMax.Y));

				FSlateDrawElement::MakeLines(
					OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(),
					LinePts, ESlateDrawEffect::None, Grid, true, 1.0f
				);

				LinePts.Reset();
				LinePts.Add(FVector2D(PlotMin.X, PlotMin.Y + T * PlotSize.Y));
				LinePts.Add(FVector2D(PlotMax.X, PlotMin.Y + T * PlotSize.Y));

				FSlateDrawElement::MakeLines(
					OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(),
					LinePts, ESlateDrawEffect::None, Grid, true, 1.0f
				);
			}

			LayerId++;
		}

		// Рамка
		{
			TArray<FVector2D> Rect;
			Rect.Add(PlotMin);
			Rect.Add(FVector2D(PlotMax.X, PlotMin.Y));
			Rect.Add(PlotMax);
			Rect.Add(FVector2D(PlotMin.X, PlotMax.Y));
			Rect.Add(PlotMin);

			FSlateDrawElement::MakeLines(
				OutDrawElements, LayerId++, AllottedGeometry.ToPaintGeometry(),
				Rect, ESlateDrawEffect::None, FLinearColor(1,1,1,0.12f), true, 1.0f
			);
		}

		// Линии
		for (const FGraphLine& L : GO->Lines)
		{
			if (!L.bVisible || L.Points.Num() < 2)
				continue;

			TArray<FVector2D> ScreenPts;
			ScreenPts.Reserve(L.Points.Num());
			for (const FVector2D& P : L.Points)
				ScreenPts.Add(ToScreen(P));

			FSlateDrawElement::MakeLines(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry(),
				ScreenPts,
				ESlateDrawEffect::None,
				L.Color,
				true,
				L.Thickness
			);

			LayerId++;
		}

		return LayerId;
	}

private:
	TWeakObjectPtr<UAdvancedVehicleGraphObject> GraphObject;
};