// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerHUDWidget.h"
#include "InventoryWidget.h"


void UPlayerHUDWidget::ToggleInventory() {
	UE_LOG(LogTemp, Warning, TEXT("asdasd"));
	if (inventory) {
		if (inventory->Visibility != ESlateVisibility::Hidden) {
			inventory->SetVisibility(ESlateVisibility::Hidden);
		}
		else {
			inventory->SetVisibility(ESlateVisibility::Visible);
		}
		

	}
	
}



