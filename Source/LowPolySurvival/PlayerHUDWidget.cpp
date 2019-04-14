// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerHUDWidget.h"
#include "InventoryWidget.h"


void UPlayerHUDWidget::NativeTick(const FGeometry &MyGeometry, float InDeltaTime) {

	if (playerController->WasInputKeyJustPressed(EKeys::Tab)) {
		UE_LOG(LogTemp, Warning, TEXT("asdasd"));
		
	}

	

}


void UPlayerHUDWidget::OpenInventory(){
	playerController->bShowMouseCursor = true;
	inventory->SetVisibility(ESlateVisibility::Visible);
	playerController->SetInputMode(FInputModeGameAndUI());
	SetUserFocus(playerController);
}

void UPlayerHUDWidget::CloseInventory(){
	playerController->bShowMouseCursor = false;
	inventory->SetVisibility(ESlateVisibility::Hidden);
	playerController->SetInputMode(FInputModeGameOnly());
}

void UPlayerHUDWidget::ToggleInventory() {
	
	if (inventory) {

		if (inventory->Visibility != ESlateVisibility::Hidden) {
			CloseInventory();
		}
		else {
			OpenInventory();
		}
		
	}
	
}





