// Fill out your copyright notice in the Description page of Project Settings.

#include "UIWidget.h"
#include "PlayercharController.h"

bool UUIWidget::Initialize() {
	Super::Initialize();

	bIsFocusable = true;

	return true;
}


void UUIWidget::OpenUI(){
	APlayercharController* playerController = Cast<APlayercharController>(GetOwningPlayer());
	if (playerController) {
		playerController->SetInputMode(FInputModeUIOnly());
		playerController->bShowMouseCursor = true;
		SetUserFocus(playerController);
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		playerController->CenterMouse();
	}
	
	bIsUIOpen = true;
}


void UUIWidget::CloseUI(){
	APlayercharController* playerController = Cast<APlayercharController>(GetOwningPlayer());
	if (playerController) {
		playerController->SetInputMode(FInputModeGameOnly());
		playerController->bShowMouseCursor = false;
		SetVisibility(ESlateVisibility::Collapsed);
	}
	bIsUIOpen = false;
}

UUIWidget* UUIWidget::SetViewportWidget(TSubclassOf<UUserWidget> widgetClass){
	RemoveFromParent();
	UUIWidget* createdWidget = CreateWidget<UUIWidget>(GetOwningPlayer(), widgetClass);
	createdWidget->AddToViewport();
	return createdWidget;
}

bool UUIWidget::IsUIOpen() const
{
	return bIsUIOpen;
}

FReply UUIWidget::NativeOnPreviewKeyDown(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent){

	if (InKeyEvent.GetKey() == EKeys::Tab || InKeyEvent.GetKey() == EKeys::Escape) {
		CloseUI();
	}

	return FReply(Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent));
}

