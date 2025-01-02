// Copyright Alexander. All Rights Reserved.

#pragma once

#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

class FAdvancedPluginEditorStyle final : public FSlateStyleSet
{
public:
	FAdvancedPluginEditorStyle() : FSlateStyleSet("AdvancedPluginEditorStyle")
	{
		const FVector2D Icon16x16(16.0f, 16.0f);
		const FVector2D Icon64x64(64.0f, 64.0f);

		SetContentRoot(FPaths::ProjectPluginsDir() / TEXT("AdvancedPhysicsTick/Resources"));

		Set("ClassIcon.AdvancedPawn", new FSlateVectorImageBrush(RootToContentDir(TEXT("AdvancedPawn_16.svg")), Icon16x16));
		Set("ClassThumbnail.AdvancedPawn", new FSlateVectorImageBrush(RootToContentDir(TEXT("AdvancedPawn_64.svg")), Icon64x64));

		FSlateStyleRegistry::RegisterSlateStyle(*this);
	}

	~FAdvancedPluginEditorStyle()
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*this);
	}

public:
	static FAdvancedPluginEditorStyle& Get()
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
	static TOptional<FAdvancedPluginEditorStyle> Singleton;
};

TOptional<FAdvancedPluginEditorStyle> FAdvancedPluginEditorStyle::Singleton;