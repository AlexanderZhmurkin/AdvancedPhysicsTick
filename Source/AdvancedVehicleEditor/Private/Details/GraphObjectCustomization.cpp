// Fill out your copyright notice in the Description page of Project Settings.

#include "Details/GraphObjectCustomization.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"

#include "AdvancedVehicleGraphObject.h"
#include "SGraph2DWidget.h"

void FGraphObjectCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);

	TWeakObjectPtr<UAdvancedVehicleGraphObject> GraphObj;
	for (const TWeakObjectPtr<UObject>& Obj : Objects)
	{
		if (UAdvancedVehicleGraphObject* GO = Cast<UAdvancedVehicleGraphObject>(Obj.Get()))
		{
			GraphObj = GO;
			break;
		}
	}

	IDetailCategoryBuilder& Cat = DetailBuilder.EditCategory("Graph", FText::FromString("Graph"), ECategoryPriority::Important);

	Cat.AddCustomRow(FText::FromString("Graph2D"))
	.WholeRowContent()
	[
		SNew(SBox)
		.MinDesiredHeight(240.f)
		.MaxDesiredHeight(420.f)
		[
			SNew(SGraph2DWidget)
			.GraphObject(GraphObj)
		]
	];

	// По желанию: оставим стандартные свойства (Lines, bounds) в той же категории
}