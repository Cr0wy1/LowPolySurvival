// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerHUDWidget.h"
#include "InventoryManagerWidget.h"
#include "QuickSlotsWidget.h"
#include "ProgressBar.h"
#include "LowPolySurvivalCharacter.h"
#include "AttributeComponent.h"
#include "HeartWidget.h"
#include "InventoryComponent.h"

void UPlayerHUDWidget::Init(FAttributes * attributes, UInventoryComponent* _equipInventoryComp){
	playerAttributes = attributes;
	equipInventoryComp = _equipInventoryComp;
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
	
}

void UPlayerHUDWidget::UpdateArmor(){
	TArray<FItemStack>* equipArr = &equipInventoryComp->GetItemStacksRef();

	int32 armorCount = 0;
	for (size_t i = 0; i < equipArr->Num(); ++i){
		if ((*equipArr)[i].IsValid()) {
			armorCount += (*equipArr)[i].itemInfo->armor;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("armor %i"), armorCount);

	heartWidget->SetArmorPercent(armorCount * 0.1);
}

int32 UPlayerHUDWidget::GetPlayerHealth() const
{
	return playerAttributes->health;
}
