// Fill out your copyright notice in the Description page of Project Settings.

#include "AdvancedVehicleEditor.h"
#include "AdvancedVehicleEditorStyle.h"
#include "Details/GraphObjectCustomization.h"
#include "AdvancedVehicleGraphObject.h"
#include "AssetToolsModule.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "HAL/ConsoleManager.h"

#define LOCTEXT_NAMESPACE "IAdvancedVehicleEditorModule"

void IAdvancedVehicleEditorModule::StartupModule()
{
	FAdvancedVehicleEditorStyle::Initialize();

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomClassLayout(
		UAdvancedVehicleGraphObject::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FGraphObjectCustomization::MakeInstance)
	);

	PropertyModule.NotifyCustomizationModuleChanged();
}

void IAdvancedVehicleEditorModule::ShutdownModule()
{
	FAdvancedVehicleEditorStyle::Shutdown();

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule =
			FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

		PropertyModule.UnregisterCustomClassLayout(UAdvancedVehicleGraphObject::StaticClass()->GetFName());
		PropertyModule.NotifyCustomizationModuleChanged();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(IAdvancedVehicleEditorModule, AdvancedVehicleEditor)
