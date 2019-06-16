// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingSlotWidget.h"
#include "Item.h"
#include "ItemStackWidget.h"
#include "Engine/DataTable.h"
#include "CraftingComponent.h"




bool UCraftingSlotWidget::Initialize() {


	return Super::Initialize();
}


void UCraftingSlotWidget::Init(FCraftingInfo* _craftingInfo, UCraftingComponent* _craftingComp) {

	craftingInfo = _craftingInfo;
	craftingComp = _craftingComp;
	itemStackWidget->Init(craftingInfo->result[0].itemInfo, craftingInfo->result[0].amount);
	
}

void UCraftingSlotWidget::OnLeftClick(const FPointerEvent & MouseEvent){
	
	craftingComp->Craft(FName(*FString::FromInt(craftingInfo->id)), 1);
}

void UCraftingSlotWidget::OnRightClick(const FPointerEvent & MouseEvent){
	
}



