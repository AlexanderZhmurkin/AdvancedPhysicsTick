// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "IDetailCustomization.h"

class FGraphObjectCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance()
	{
		return MakeShareable(new FGraphObjectCustomization());
	}

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};