// Fill out your copyright notice in the Description page of Project Settings.

#include "HeartWidget.h"
#include "ProgressBar.h"
#include "TextBlock.h"
#include "Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void UHeartWidget::SetHeartValues(float percentProgress, int32 amountText){
	heartProgress->SetPercent(percentProgress);
	heartText->SetText( FText::FromString( FString::FromInt(amountText) ) );
}

void UHeartWidget::SetArmorPercent(float percent){
	UMaterialInstanceDynamic* armorMaterial = armorImage->GetDynamicMaterial();
	if (armorMaterial) {
		armorMaterial->SetScalarParameterValue("Alpha", percent);
	}
	
}
