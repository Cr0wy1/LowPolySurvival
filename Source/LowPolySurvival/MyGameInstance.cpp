// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "Engine/DataTable.h"
#include "Item.h"

void UMyGameInstance::Init(){
	Super::Init();

	if (!itemDataTable || !craftingDataTable || !dropsDataTable) {
		UE_LOG(LogTemp, Error, TEXT("MyGameInstance: Missing DataTable"));
	}

	InitDataTables();

}

void UMyGameInstance::InitDataTables(){
	if (!itemDataTable || !craftingDataTable) return;


	TArray<FCraftingInfo*> craftingInfos;
	craftingDataTable->GetAllRows(FString(), craftingInfos);

	for (FCraftingInfo* craftingInfo : craftingInfos) {

		for (FCraftPart& neededPart : craftingInfo->needed) {
			neededPart.itemInfo = itemDataTable->FindRow<FItemInfo>(FName(*FString::FromInt(neededPart.id)), FString());
		}

		for (FCraftPart& resultPart : craftingInfo->result) {
			resultPart.itemInfo = itemDataTable->FindRow<FItemInfo>(FName(*FString::FromInt(resultPart.id)), FString());
		}

	}

	if (!dropsDataTable) return;
	TArray<FItemDrops*> itemDrops;
	dropsDataTable->GetAllRows(FString(), itemDrops);

	for (FItemDrops* itemDrop : itemDrops) {
		for (FDropInfo& dropInfo : itemDrop->itemId) {
			dropInfo.itemInfo = itemDataTable->FindRow<FItemInfo>(dropInfo.itemId, FString());
		}
	}

}

UDataTable * UMyGameInstance::GetItemTable() const{

	return itemDataTable;
}

UDataTable * UMyGameInstance::GetDropsTable() const{

	return dropsDataTable;
}

UDataTable * UMyGameInstance::GetCraftingTable() const
{
	return craftingDataTable;
}
