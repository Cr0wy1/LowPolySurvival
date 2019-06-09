// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerHUDWidget.h"
#include "InventoryManagerWidget.h"
#include "QuickSlotsWidget.h"
#include "ProgressBar.h"
#include "LowPolySurvivalCharacter.h"
#include "AttributeComponent.h"
#include "HeartWidget.h"
#include "InventoryComponent.h"
#include "Buildings.h"
#include "TextBlock.h"
#include "PanelWidget.h"

void UPlayerHUDWidget::Init(FAttributes * attributes, UInventoryComponent* _equipInventoryComp){
	playerAttributes = attributes;
	equipInventoryComp = _equipInventoryComp;
	equipInventoryComp->OnInventoryUpdate.AddDynamic(this, &UPlayerHUDWidget::OnEquipCompUpdate);

	UpdateHealth();
	UpdateArmor();
}

void UPlayerHUDWidget::BindQuickSlot(UInventoryComponent *quickInvComp, UInventoryManagerWidget* _inventoryManager){
	playerQuickInv->Init(_inventoryManager, EWidgetInvType::QUICKSLOTS, quickInvComp);
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

	//UE_LOG(LogTemp, Warning, TEXT("armor %i"), armorCount);

	heartWidget->SetArmorPercent((float)armorCount/16.0f);
}

void UPlayerHUDWidget::UpdateTargetIndicator(const FBuildingInfo &targetInfo){
	damageIndicatorProgress->SetPercent((float)targetInfo.currentDurability / (float)targetInfo.itemInfo->durability);
	nameIndicatorText->SetText(FText::FromName(targetInfo.itemInfo->name));
}

void UPlayerHUDWidget::ShowTargetIndicator(){
	indicatorBox->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UPlayerHUDWidget::HideTargetIndicator(){
	indicatorBox->SetVisibility(ESlateVisibility::Collapsed);
	
}

void UPlayerHUDWidget::OnEquipCompUpdate(const TArray<int32> &updatedSlots){
	UpdateArmor();
}

int32 UPlayerHUDWidget::GetPlayerHealth() const
{
	return playerAttributes->health;
}

int32 UPlayerHUDWidget::GetCurrentSelectedQuickSlot() const
{
	return playerQuickInv->GetCurrentSelectedSlot();
}
