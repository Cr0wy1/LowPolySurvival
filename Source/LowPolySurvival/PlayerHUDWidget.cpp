// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerHUDWidget.h"
#include "InventoryManagerWidget.h"
#include "QuickSlotsWidget.h"
#include "ProgressBar.h"
#include "LowPolySurvivalCharacter.h"
#include "AttributeComponent.h"
#include "HeartWidget.h"

void UPlayerHUDWidget::Init(FAttributes * attributes){
	playerAttributes = attributes;
}

void UPlayerHUDWidget::BindQuickSlot(UInventoryComponent *quickInvComp, UInventoryManagerWidget* _inventoryManager){
	playerQuickInv->Init(_inventoryManager, true, quickInvComp);
}

FItemStack* UPlayerHUDWidget::OnScrollDown(){
	return playerQuickInv->SelectNextSlot();
}

FItemStack* UPlayerHUDWidget::OnScrollUp(){
	return playerQuickInv->SelectPreviousSlot();
}

void UPlayerHUDWidget::UpdateHealth(){
	heartWidget->SetHeartValues(playerAttributes->health * 0.01, playerAttributes->health);
	heartWidget->SetArmorPercent(playerAttributes->health * 0.01);
}

int32 UPlayerHUDWidget::GetPlayerHealth() const
{
	return playerAttributes->health;
}
