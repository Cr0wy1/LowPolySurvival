// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "Engine/DataTable.h"
#include "Item.h"
#include "WorldGenerator.h"
#include "Engine/World.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"

const float FWorldParams::blockSize = 100.0f;
const float FWorldParams::deathZone = 0.0f;
const float FWorldParams::buildHeight = 10000.0f;

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


AWorldGenerator * UMyGameInstance::GetWorldGenerator() const{

	return worldGenerator;
}

FVector WorldToBlockLocation(const FVector & worldLocation){

	FVector result = worldLocation / FWorldParams::blockSize;
	result.X = FMath::FloorToFloat(result.X);
	result.Y = FMath::FloorToFloat(result.Y);
	result.Z = FMath::FloorToFloat(result.Z);

	return result;
}  

FVector2D WorldToChunkLocation(const FVector & worldLocation){ 

	FVector2D result = FVector2D(worldLocation / (FWorldParams::blockSize*FWorldParams::chunkSize) );
	result.X = FMath::FloorToFloat(result.X); 
	result.Y = FMath::FloorToFloat(result.Y);

	//UE_LOG(LogTemp, Warning, TEXT("worldLocation: %s, ChunkLocation: %s"), *worldLocation.ToCompactString(), *result.ToString());


	return result;
}

FVector BlockToWorldLocation(const FVector & blockLocation){

	return blockLocation * FWorldParams::blockSize;
}

FVector ChunkToWorldLocation(const FIntVector & chunkLocation){

	return FVector(chunkLocation * FWorldParams::blockSize*FWorldParams::chunkSize);
}

FVector ChunkToBlockLocation(const FIntVector & chunkLocation){

	return FVector(chunkLocation * FWorldParams::chunkSize);
}

TArray<FIntVector> BlockToChunkBlockLocation(const FIntVector &blockLocation, TArray<FIntVector> &OUT_chunkBlockLocation) {

	TArray<FIntVector> result;

	FIntVector chunkLoc(FVector(blockLocation) / FWorldParams::chunkSize);
	chunkLoc.X = FMath::FloorToFloat(chunkLoc.X);
	chunkLoc.Y = FMath::FloorToFloat(chunkLoc.Y);
	chunkLoc.Z = 0;

	FIntVector chunkBlockLoc = FIntVector(blockLocation.X % FWorldParams::chunkSize, blockLocation.Y % FWorldParams::chunkSize, blockLocation.Z);

	result.Add(chunkLoc);
	OUT_chunkBlockLocation.Add(chunkBlockLoc);

	if (chunkBlockLoc.X == 0) {
		result.Add(FIntVector(chunkLoc.X - 1, chunkLoc.Y, 0) );
		OUT_chunkBlockLocation.Add(FIntVector(FWorldParams::chunkSize-1, chunkBlockLoc.Y, chunkBlockLoc.Z));
	}

	if (chunkBlockLoc.Y == 0) {
		result.Add(FIntVector(chunkLoc.X, chunkLoc.Y - 1, 0));
		OUT_chunkBlockLocation.Add(FIntVector(chunkBlockLoc.X, FWorldParams::chunkSize-1, chunkBlockLoc.Z));
	}
	
	if (result.Num() == 3) {
		result.Add(FIntVector(chunkLoc.X - 1, chunkLoc.Y - 1, 0));
		OUT_chunkBlockLocation.Add(FIntVector(FWorldParams::chunkSize-1, FWorldParams::chunkSize-1, chunkBlockLoc.Z));
	}

	return result;
} 
 