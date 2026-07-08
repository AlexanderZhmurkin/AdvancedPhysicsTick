// Fill out your copyright notice in the Description page of Project Settings.

#include "AdvancedVehicleEditorStyle.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Brushes/SlateImageBrush.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"

TSharedPtr<FSlateStyleSet> FAdvancedVehicleEditorStyle::StyleInstance = nullptr;

void FAdvancedVehicleEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FAdvancedVehicleEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

const ISlateStyle& FAdvancedVehicleEditorStyle::Get()
{
	return *StyleInstance;
}

FName FAdvancedVehicleEditorStyle::GetStyleSetName()
{
	return TEXT("AdvancedVehicleEditorStyle");
}

TSharedRef<FSlateStyleSet> FAdvancedVehicleEditorStyle::Create()
{
	TSharedPtr<IPlugin> PluginPath = IPluginManager::Get().FindPlugin(TEXT("AdvancedVehicle"));
	check(PluginPath.IsValid());

	const FString PluginRoot = PluginPath->GetBaseDir();
	const FString ContentRoot = FPaths::ConvertRelativePathToFull(PluginRoot / TEXT("Resources"));

	TSharedRef<FSlateStyleSet> StyleRef = MakeShareable(new FSlateStyleSet(FAdvancedVehicleEditorStyle::GetStyleSetName()));
	StyleRef->SetContentRoot(ContentRoot);

	FSlateStyleSet& Style = StyleRef.Get();

	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);
	const FVector2D Icon64x64(64.0f, 64.0f);

	Style.Set("ClassIcon.AdvancedVehiclePawn_Skeletal", new FSlateVectorImageBrush(Style.RootToContentDir(TEXT("AdvancedVehiclePawnSkel_16.svg")), Icon16x16));
	Style.Set("ClassThumbnail.AdvancedVehiclePawn_Skeletal", new FSlateVectorImageBrush(Style.RootToContentDir(TEXT("AdvancedVehiclePawnSkel_64.svg")), Icon64x64));

	Style.Set("ClassIcon.AdvancedVehiclePawn_Static", new FSlateVectorImageBrush(Style.RootToContentDir(TEXT("AdvancedVehiclePawnStatic_16.svg")), Icon16x16));
	Style.Set("ClassThumbnail.AdvancedVehiclePawn_Static", new FSlateVectorImageBrush(Style.RootToContentDir(TEXT("AdvancedVehiclePawnStatic_64.svg")), Icon64x64));
	
	Style.Set("ClassIcon.AdvancedVehiclePawn", new FSlateVectorImageBrush(Style.RootToContentDir(TEXT("AdvancedVehiclePawn_16.svg")), Icon16x16));
	Style.Set("ClassThumbnail.AdvancedVehiclePawn", new FSlateVectorImageBrush(Style.RootToContentDir(TEXT("AdvancedVehiclePawn_64.svg")), Icon64x64));

	Style.Set("ClassIcon.AdvancedMovement", new FSlateVectorImageBrush(Style.RootToContentDir(TEXT("AdvancedMovementComponent_16.svg")), Icon16x16));
	Style.Set("ClassThumbnail.AdvancedMovement", new FSlateVectorImageBrush(Style.RootToContentDir(TEXT("AdvancedMovementComponent_64.svg")), Icon64x64));

	return StyleRef;
}