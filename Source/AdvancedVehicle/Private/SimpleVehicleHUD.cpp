// Fill out your copyright notice in the Description page of Project Settings.

#include "SimpleVehicleHUD.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/HUD.h"
#include "DrawDebugHelpers.h"
#include "DisplayDebugHelpers.h"

static bool bDebugShowEnabled = false;

static FAutoConsoleVariableRef CVarVehicleHUDDebugTelemetry(
	TEXT("r.SimpleVehicleHUD.DebugTelemetry"),
	bDebugShowEnabled,
	TEXT("Enables display of all debug telemetry information on the HUD")
);

ASimpleVehicleHUD::ASimpleVehicleHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ASimpleVehicleHUD::BeginPlay()
{
	Super::BeginPlay();

	AHUD::OnHUDPostRender.AddUObject(this, &ASimpleVehicleHUD::OnHUDPostRender);
}

void ASimpleVehicleHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	AHUD::OnHUDPostRender.RemoveAll(this);
}

void ASimpleVehicleHUD::OnHUDPostRender(AHUD* InHUD, UCanvas* InCanvas)
{
	if (bDebugShowEnabled && InCanvas)
	{
		if (PlayerOwner && PlayerOwner->IsLocalController())
		{
			DrawDebug2D(InCanvas, InCanvas->DisplayDebugManager.GetMaxCharHeightRef(), InCanvas->DisplayDebugManager.GetYPosRef());
		}
	}
}

void ASimpleVehicleHUD::DrawDebug2D(UCanvas* InCanvas, float& YL, float& YPos)
{
	UFont* RenderFont = GEngine->GetMediumFont();
	FFontRenderInfo RenderInfo;
	RenderInfo.bEnableShadow = true;

	YPos = 10;
	InCanvas->SetDrawColor(FColor::White);

	BlueprintDrawDebug2D(InCanvas, YL, YPos);
}

void ASimpleVehicleHUD::DrawDebugGraph(UCanvas* InCanvas, const FString& Title, float ValueX, float ValueY, float UL_X, float UL_Y, float W, float H, FVector2D RangeX, FVector2D RangeY)
{
	InCanvas->DrawDebugGraph(Title, ValueX, ValueY, UL_X, UL_Y, W, H, RangeX, RangeY);
}

void ASimpleVehicleHUD::DrawDebugAdvancedGraph(UCanvas* InCanvas, const FString& Title, const TArray<FDebugGraphPoint>& Points, float UL_X, float UL_Y, float W, float H, FVector2D RangeX, FVector2D RangeY, const FString& TitleX, const FString& TitleY, int32 GridX, int32 GridY)
{
    static const int32 GRAPH_ICONSIZE = 8;

    const UFont* Font = GEngine->GetSmallFont();

    const float SafeRangeX = (RangeX.Y - RangeX.X) != 0 ? (RangeX.Y - RangeX.X) : 1.0f;
    const float SafeRangeY = (RangeY.Y - RangeY.X) != 0 ? (RangeY.Y - RangeY.X) : 1.0f;

    FCanvasBoxItem Box(FVector2D(UL_X, UL_Y), FVector2D(W, H));
    Box.SetColor(FLinearColor::White);
    InCanvas->DrawItem(Box);

    for (int32 i = 1; i < GridX; i++)
    {
        const float GX = UL_X + (W / GridX) * i;
        FCanvasLineItem Line(FVector2D(GX, UL_Y), FVector2D(GX, UL_Y + H));
        Line.SetColor(FLinearColor(0.2f, 0.2f, 0.2f, 0.6f));
        InCanvas->DrawItem(Line);
    }

    for (int32 i = 1; i < GridY; i++)
    {
        const float GY = UL_Y + (H / GridY) * i;
        FCanvasLineItem Line(FVector2D(UL_X, GY), FVector2D(UL_X + W, GY));
        Line.SetColor(FLinearColor(0.2f, 0.2f, 0.2f, 0.6f));
        InCanvas->DrawItem(Line);
    }

    for (const FDebugGraphPoint& P : Points)
    {
        const float NormX = FMath::Clamp((P.Value.X - RangeX.X) / SafeRangeX, 0.0f, 1.0f);
        const float NormY = FMath::Clamp((P.Value.Y - RangeY.X) / SafeRangeY, 0.0f, 1.0f);

        const float X = UL_X + NormX * W;
        const float Y = UL_Y + NormY * H;

        FCanvasTileItem Tile(
            FVector2D(X - GRAPH_ICONSIZE * 0.5f, Y - GRAPH_ICONSIZE * 0.5f),
            GWhiteTexture,
            FVector2D(GRAPH_ICONSIZE, GRAPH_ICONSIZE),
            P.Color);

        InCanvas->DrawItem(Tile);

        if (!P.Title.IsEmpty())
        {
            FCanvasTextItem TextItem(
                FVector2D(X + GRAPH_ICONSIZE + 2, Y - 6),
                FText::FromString(P.Title),
                Font,
                P.Color);

            InCanvas->DrawItem(TextItem);
        }
    }

    FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), Font, FColor::White);
    float TextW, TextH;
    InCanvas->StrLen(Font, TitleX, TextW, TextH);
    TextItem.Text = FText::FromString(TitleX);
    const float CenterX = UL_X + W * 0.5f - TextW * 0.5f;
    InCanvas->DrawItem(TextItem, FVector2D(CenterX, UL_Y + H + 32));

    TextItem.Text = FText::FromString(TitleY);
    InCanvas->DrawItem(TextItem, FVector2D(UL_X - 30, UL_Y + H * 0.5f - 6));

    if (!Title.IsEmpty())
    {
        TextItem.Text = FText::FromString(Title);
        TextItem.Scale = FVector2D(1.15f, 1.15f);
        InCanvas->DrawItem(TextItem, FVector2D(UL_X, UL_Y - 18));
    }
}

void ASimpleVehicleHUD::DrawDebugCircle(UCanvas* InCanvas, FVector2D Center, FVector2D StartPos, FLinearColor Color, int32 Segments, float CircleRadius, float LineThickness)
{
	for (int32 i = 0; i < Segments; ++i)
	{
		const float Angle1 = (2 * PI * i) / Segments;
		const float Angle2 = (2 * PI * (i + 1)) / Segments;

		const FVector2D P1 = Center + FVector2D(FMath::Cos(Angle1), FMath::Sin(Angle1)) * CircleRadius;
		const FVector2D P2 = Center + FVector2D(FMath::Cos(Angle2), FMath::Sin(Angle2)) * CircleRadius;

		InCanvas->K2_DrawLine(P1, P2, LineThickness, Color);
	}
}