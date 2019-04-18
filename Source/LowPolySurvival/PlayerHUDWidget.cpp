// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerHUDWidget.h"
#include "InventoryManagerWidget.h"
#include "QuickSlotsWidget.h"

void UPlayerHUDWidget::BindQuickSlot(UInventoryComponent *quickInvComp, UInventoryManagerWidget* _inventoryManager){
	playerQuickInv->Init(_inventoryManager, true, quickInvComp);
}

FItemStack* UPlayerHUDWidget::OnScrollDown(){
	return playerQuickInv->SelectNextSlot();
}

FItemStack* UPlayerHUDWidget::OnScrollUp(){
	return playerQuickInv->SelectPreviousSlot();
}
