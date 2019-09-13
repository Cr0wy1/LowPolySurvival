// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "Engine/DataTable.h"
#include "Item.h"
#include "WorldGenerator.h"
#include "Engine/World.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "WidgetAsset.h"
#include "BiomeGenLibrary.h"

const float FWorldParams::blockSize = 100.0f;
const float FWorldParams::buildHeight = 10000.0f;
const int32 FWorldParams::deathZone = 0.0f;


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

	if (itemDataTable && craftingDataTable) {
		TArray<FCraftingInfo*> craftingInfos;
		craftingDataTable->GetAllRows(FString(), craftingInfos);

		for (FCraftingInfo* craftingInfo : craftingInfos) {

			for (FCraftPart& neededPart : craftingInfo->needed) {
				neededPart.itemInfo = itemDataTable->FindRow<FItemData>(FName(*FString::FromInt(neededPart.id)), FString());
			}

			for (FCraftPart& resultPart : craftingInfo->result) {
				resultPart.itemInfo = itemDataTable->FindRow<FItemData>(FName(*FString::FromInt(resultPart.id)), FString());
			}

		}
	}

	if (itemDataTable && dropsDataTable) {
		TArray<FItemDrops*> itemDrops;
		dropsDataTable->GetAllRows(FString(), itemDrops);

		for (FItemDrops* itemDrop : itemDrops) {
			for (FDropInfo& dropInfo : itemDrop->itemId) {
				dropInfo.itemInfo = itemDataTable->FindRow<FItemData>(dropInfo.itemId, FString());
			}
		}
	}

	if (biomeDataTable && resourceDataTable) {
		TArray<FBiomeData*> biomes;
		biomeDataTable->GetAllRows(FString(), biomes);

		for (FBiomeData* biomeData : biomes) {
			biomeData->baseBlockResource = resourceDataTable->FindRow<FResource>(biomeData->baseBlockResourceId, FString());
			if (!biomeData->baseBlockResource) {
				biomeData->baseBlockResource = resourceDataTable->FindRow<FResource>("0", FString());
			}
		}

		FSBiome::InitStaticMembers(this);
		FSBiomeDiagram::InitStaticMembers();
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

UDataTable * UMyGameInstance::GetCraftingTable() const{
	return craftingDataTable;
}

UDataTable * UMyGameInstance::GetIslandTable() const{
	return islandDataTable;
}

UDataTable * UMyGameInstance::GetWorldGenTable() const{
	return worldGenDataTable;
}

UDataTable * UMyGameInstance::GetResourceTable() const{
	return resourceDataTable;
}

UDataTable * UMyGameInstance::GetBiomeTable() const{
	return biomeDataTable;
}


AWorldGenerator * UMyGameInstance::GetWorldGenerator() const{

	return worldGenerator;
}

UWidgetAsset * UMyGameInstance::GetWidgetAsset() const
{
	return widgetAsset_A;
}


FIntVector WorldToBlockLocation(const FVector & worldLocation){

	FVector result = worldLocation / FWorldParams::blockSize;
	result.X = FMath::FloorToFloat(result.X);
	result.Y = FMath::FloorToFloat(result.Y);
	result.Z = FMath::FloorToFloat(result.Z);

	return FIntVector(result);
}  

FIntVector WorldToChunkLocation(const FVector & worldLocation){

	FVector result = FVector(worldLocation / (FWorldParams::blockSize*FWorldParams::chunkSize) );
	result.X = FMath::FloorToFloat(result.X); 
	result.Y = FMath::FloorToFloat(result.Y);
	result.Z = FMath::FloorToFloat(result.Z);

	//UE_LOG(LogTemp, Warning, TEXT("worldLocation: %s, ChunkLocation: %s"), *worldLocation.ToCompactString(), *result.ToString());


	return FIntVector(result);
}

FVector BlockToWorldLocation(const FVector & blockLocation){

	return blockLocation * FWorldParams::blockSize;
}

FIntVector BlockToChunkLocation(const FIntVector & blockLocation){

	FIntVector result(blockLocation.X >> 4, blockLocation.Y >> 4, blockLocation.Z >> 4);
	//FVector floatLoc(blockLocation);

	//floatLoc /= FWorldParams::chunkSize;

	//result.X = FMath::FloorToInt(floatLoc.X);
	//result.Y = FMath::FloorToInt(floatLoc.Y);
	//result.Z = FMath::FloorToInt(floatLoc.Z);

	return result;
}

FVector ChunkToWorldLocation(const FIntVector & chunkLocation){

	return FVector(chunkLocation * FWorldParams::blockSize*FWorldParams::chunkSize);
}

FIntVector ChunkToBlockLocation(const FIntVector & chunkLocation){

	return chunkLocation * FWorldParams::chunkSize;
}

FIntVector BlockToChunkBlockLocation(const FIntVector & blockLocation, FIntVector &OUT_chunkBlockLocation){

	FIntVector chunkLoc = BlockToChunkLocation(blockLocation);

	OUT_chunkBlockLocation = FIntVector(blockLocation.X & 15, blockLocation.Y & 15, blockLocation.Z & 15);

	return chunkLoc;
}

TArray<FIntVector> BlockToChunkBlockLocation(const FIntVector &blockLocation, TArray<FIntVector> &OUT_chunkBlockLocation) {

	TArray<FIntVector> result;

	FIntVector chunkBlockLoc;

	FIntVector chunkLoc = BlockToChunkBlockLocation(blockLocation, chunkBlockLoc);


	result.Add(chunkLoc);
	OUT_chunkBlockLocation.Add(chunkBlockLoc);

	bool bXAtEdge = chunkBlockLoc.X == 0;
	bool bYAtEdge = chunkBlockLoc.Y == 0;
	bool bZAtEdge = chunkBlockLoc.Z == 0;

	if (bXAtEdge) {
		result.Add(FIntVector(chunkLoc.X - 1, chunkLoc.Y, chunkLoc.Z) );
		OUT_chunkBlockLocation.Add(FIntVector(FWorldParams::chunkSize-1, chunkBlockLoc.Y, chunkBlockLoc.Z));
	}

	if (bYAtEdge) {
		result.Add(FIntVector(chunkLoc.X, chunkLoc.Y - 1, chunkLoc.Z));
		OUT_chunkBlockLocation.Add(FIntVector(chunkBlockLoc.X, FWorldParams::chunkSize-1, chunkBlockLoc.Z));
	}

	if (bZAtEdge) {
		result.Add(FIntVector(chunkLoc.X, chunkLoc.Y, chunkLoc.Z - 1));
		OUT_chunkBlockLocation.Add(FIntVector(chunkBlockLoc.X, chunkBlockLoc.Y, FWorldParams::chunkSize - 1));
	}

	if (bXAtEdge && bYAtEdge) {
		result.Add(FIntVector(chunkLoc.X - 1, chunkLoc.Y - 1, chunkLoc.Z));
		OUT_chunkBlockLocation.Add(FIntVector(FWorldParams::chunkSize - 1, FWorldParams::chunkSize - 1, chunkBlockLoc.Z));
	}

	if (bXAtEdge && bZAtEdge) {
		result.Add(FIntVector(chunkLoc.X - 1, chunkLoc.Y, chunkLoc.Z - 1));
		OUT_chunkBlockLocation.Add(FIntVector(FWorldParams::chunkSize - 1, chunkBlockLoc.Y, FWorldParams::chunkSize - 1));
	}

	if (bYAtEdge && bZAtEdge) {
		result.Add(FIntVector(chunkLoc.X, chunkLoc.Y - 1, chunkLoc.Z - 1));
		OUT_chunkBlockLocation.Add(FIntVector(chunkBlockLoc.X, FWorldParams::chunkSize - 1, FWorldParams::chunkSize - 1));
	}

	if (bXAtEdge && bYAtEdge && bZAtEdge) {
		result.Add(FIntVector(chunkLoc.X - 1, chunkLoc.Y - 1, chunkLoc.Z - 1));
		OUT_chunkBlockLocation.Add(FIntVector(FWorldParams::chunkSize - 1, FWorldParams::chunkSize - 1, FWorldParams::chunkSize - 1));
	}

	


	return result;
} 
 