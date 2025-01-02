// Copyright Alexander. All Rights Reserved.

#include "AdvancedPluginEditor.h"
#include "AdvancedPluginEditorStyle.h"
#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "HAL/ConsoleManager.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "IAdvancedPluginEditorModule"

void IAdvancedPluginEditorModule::StartupModule()
{
	FAdvancedPluginEditorStyle::Get();
}

void IAdvancedPluginEditorModule::ShutdownModule()
{
	FAdvancedPluginEditorStyle::Destroy();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(IAdvancedPluginEditorModule, AdvancedPluginEditor)