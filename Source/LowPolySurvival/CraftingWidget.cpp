// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingWidget.h"
#include "MyGameInstance.h"
#include "Engine/DataTable.h"
#include "Item.h"
#include "ItemSlotGridWidget.h"
#include "CraftingSlotWidget.h"
#include "CraftingComponent.h"

void UCraftingWidget::Init(UCraftingComponent* _craftingComp) {

	craftingComp = _craftingComp;

	UMyGameInstance* gameInstance = GetGameInstance<UMyGameInstance>();

	UDataTable* craftingTable = gameInstance->GetCraftingTable();

	craftingTable->GetAllRows<FCraftingInfo>(FString(), craftingInfoArr);

	
	for (FCraftingInfo* craftingInfo : craftingInfoArr){

		UCraftingSlotWidget* craftingSlotWidget = Cast<UCraftingSlotWidget>(slotGrid->AddSlot());
		craftingSlotWidget->Init(craftingInfo, _craftingComp);
		

		//UE_LOG(LogTemp, Warning, TEXT("%i"), craftingInfoArr[i]->id);

	}
}