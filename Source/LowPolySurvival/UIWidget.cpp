// Fill out your copyright notice in the Description page of Project Settings.

#include "UIWidget.h"

bool UUIWidget::Initialize() {
	Super::Initialize();

	return true;
}


void UUIWidget::OpenUI(){
	APlayerController* playerController = GetOwningPlayer();
	if (playerController) {
		playerController->SetInputMode(FInputModeGameAndUI());
		playerController->bShowMouseCursor = true;
		SetUserFocus(playerController);
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UUIWidget::CloseUI(){
	APlayerController* playerController = GetOwningPlayer();
	if (playerController) {
		playerController->SetInputMode(FInputModeGameOnly());
		playerController->bShowMouseCursor = false;
		SetVisibility(ESlateVisibility::Collapsed);
	}
}
