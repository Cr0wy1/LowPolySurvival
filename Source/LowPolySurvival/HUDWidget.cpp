// Fill out your copyright notice in the Description page of Project Settings.

#include "HUDWidget.h"




bool UHUDWidget::Initialize(){

	Super::Initialize();

	//bIsFocusable = false;

	return true;
}

void UHUDWidget::NativeConstruct() {
	
	Super::NativeConstruct();

	playerController = GetOwningPlayer();

	if (!playerController) {
		UE_LOG(LogTemp, Error, TEXT("HUDWidget playerController = nullptr"));
	}

	
}