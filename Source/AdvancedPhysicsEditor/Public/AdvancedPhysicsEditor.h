// Fill out your copyright notice in the Description page of Project Settings.
// Created by: Alexander Zhmurkin

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "PhysicsInterfaceDeclaresCore.h"

class IAdvancedPhysicsEditorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};