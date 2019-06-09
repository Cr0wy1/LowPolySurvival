// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingWidget.h"
#include "MyGameInstance.h"
#include "Engine/DataTable.h"
#include "Item.h"
#include "ItemSlotGridWidget.h"
#include "SlotWidget.h"

void UCraftingWidget::Init() {

	UMyGameInstance* gameInstance = GetGameInstance<UMyGameInstance>();

	UDataTable* craftingTable = gameInstance->GetCraftingTable();

	craftingTable->GetAllRows<FCraftingInfo>(FString(), craftingInfoArr);

	
	for (size_t i = 0; i < craftingInfoArr.Num(); i++){

		USlotWidget* slotWidget = slotGrid->AddSlot();
		slotWidget->Init();

		UE_LOG(LogTemp, Warning, TEXT("%i"), craftingInfoArr[i]->id);

	}
}