// Fill out your copyright notice in the Description page of Project Settings.
// Created by: Alexander Zhmurkin

#include "AdvancedPhysicsEditor.h"
#include "AssetToolsModule.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "HAL/ConsoleManager.h"

#define LOCTEXT_NAMESPACE "IAdvancedPhysicsEditorModule"

class FAdvancedPhysicsEditorStyle final : public FSlateStyleSet
{
public:
	FAdvancedPhysicsEditorStyle() : FSlateStyleSet("AdvancedPhysicsEditorStyle")
	{
		const FVector2D Icon16x16(16.0f, 16.0f);
		const FVector2D Icon64x64(64.0f, 64.0f);

		const FString PluginPath = FPaths::GetPath(FModuleManager::Get().GetModuleFilename("AdvancedPhysicsEditor"));
		const FString ContentRoot = FPaths::ConvertRelativePathToFull(PluginPath / TEXT("Resources"));

		SetContentRoot(ContentRoot);

		Set("ClassIcon.AdvancedPawn", new FSlateVectorImageBrush(RootToContentDir(TEXT("AdvancedPawn_16.svg")), Icon16x16));
		Set("ClassThumbnail.AdvancedPawn", new FSlateVectorImageBrush(RootToContentDir(TEXT("AdvancedPawn_64.svg")), Icon64x64));

		Set("ClassIcon.AdvancedMovementComponent", new FSlateVectorImageBrush(RootToContentDir(TEXT("AdvancedPawn_16.svg")), Icon16x16));
		Set("ClassThumbnail.AdvancedMovementComponent", new FSlateVectorImageBrush(RootToContentDir(TEXT("AdvancedPawn_64.svg")), Icon64x64));

		FSlateStyleRegistry::RegisterSlateStyle(*this);
	}

	~FAdvancedPhysicsEditorStyle()
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*this);
	}

public:
	static FAdvancedPhysicsEditorStyle& Get()
	{
		if (!Singleton.IsSet())
		{
			Singleton.Emplace();
		}
		
		return Singleton.GetValue();
	}

	static void Destroy()
	{
		Singleton.Reset();
	}

private:
	static TOptional<FAdvancedPhysicsEditorStyle> Singleton;
};

TOptional<FAdvancedPhysicsEditorStyle> FAdvancedPhysicsEditorStyle::Singleton;

void IAdvancedPhysicsEditorModule::StartupModule()
{
	FAdvancedPhysicsEditorStyle::Get();
}

void IAdvancedPhysicsEditorModule::ShutdownModule()
{
	FAdvancedPhysicsEditorStyle::Destroy();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(IAdvancedPhysicsEditorModule, AdvancedPhysicsEditor)
