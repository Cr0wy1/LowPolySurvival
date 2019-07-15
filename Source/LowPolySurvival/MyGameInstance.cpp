// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "Engine/DataTable.h"
#include "Item.h"
#include "WorldGenerator.h"
#include "Engine/World.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"

void UMyGameInstance::Init(){
	Super::Init();

	if (!itemDataTable || !craftingDataTable || !dropsDataTable) {
		UE_LOG(LogTemp, Error, TEXT("MyGameInstance: Missing DataTable"));
	}

	InitDataTables();

	if (worldGenerator_BP) {
		worldGenerator = GetWorld()->SpawnActor<AWorldGenerator>(worldGenerator_BP);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("MyGameInstance: Missing worldGenerator_BP"));
	}
	
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

void UMyGameInstance::CreateWorld(FName worldName){
	//UGameplayStatics::CreateSaveGameObject()
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

UDataTable * UMyGameInstance::GetIslandTable() const
{
	return islandDataTable;
}

UDataTable * UMyGameInstance::GetWorldGenTable() const
{
	return worldGenDataTable;
}

FWorldInfo const*const UMyGameInstance::GetWorldInfo() const{
	return worldInfo;
}

AWorldGenerator * UMyGameInstance::GetWorldGenerator() const{

	return worldGenerator;
}

FVector WorldToBlockLocation(const FVector & worldLocation){

	FVector result = worldLocation * 0.01f;
	result.X = FMath::RoundToFloat(result.X);
	result.Y = FMath::RoundToFloat(result.Y);
	result.Z = FMath::RoundToFloat(result.Z);

	return result;
}  

FVector2D WorldToChunkLocation(const FVector & worldLocation){ 

	FVector2D result = FVector2D(worldLocation * 0.001).IntPoint(); 
	//result.X = FMath::RoundToFloat(result.X);
	//result.Y = FMath::RoundToFloat(result.Y);

	return result;
}

FVector BlockToWorldLocation(const FVector & blockLocation){

	return blockLocation * 100;
}

FVector ChunkToWorldLocation(const FVector2D & chunkLocation){

	return FVector(chunkLocation * 1000, 0);
}

FVector ChunkToBlockLocation(const FVector2D & chunkLocation){

	return FVector(chunkLocation * 10, 0);
}

TArray<FVector2D> BlockToChunkBlockLocation(const FIntVector &blockLocation, TArray<FIntVector> &OUT_chunkBlockLocation) {

	TArray<FVector2D> result;

	FVector2D chunkLoc(FVector(blockLocation) * 0.1);
	chunkLoc.X = FMath::FloorToFloat(chunkLoc.X);
	chunkLoc.Y = FMath::FloorToFloat(chunkLoc.Y);

	FIntVector chunkBlockLoc = FIntVector(blockLocation.X % 10, blockLocation.Y % 10, blockLocation.Z);

	result.Add(chunkLoc);
	OUT_chunkBlockLocation.Add(chunkBlockLoc);

	if (chunkBlockLoc.X == 0) {
		result.Add(FVector2D(chunkLoc.X - 1, chunkLoc.Y) );
		OUT_chunkBlockLocation.Add(FIntVector(chunkBlockLoc.X + 10, chunkBlockLoc.Y, chunkBlockLoc.Z));
	}

	if (chunkBlockLoc.Y == 0) {
		result.Add(FVector2D(chunkLoc.X, chunkLoc.Y - 1));
		OUT_chunkBlockLocation.Add(FIntVector(chunkBlockLoc.X, chunkBlockLoc.Y + 10, chunkBlockLoc.Z));
	}
	
	if (result.Num() == 3) {
		result.Add(FVector2D(chunkLoc.X - 1, chunkLoc.Y - 1));
		OUT_chunkBlockLocation.Add(FIntVector(chunkBlockLoc.X + 10, chunkBlockLoc.Y + 10, chunkBlockLoc.Z));
	}

	return result;
}
