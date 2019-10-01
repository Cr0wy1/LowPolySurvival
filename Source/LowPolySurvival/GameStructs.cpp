// Fill out your copyright notice in the Description page of Project Settings.

#include "GameStructs.h"
#include "MyGameInstance.h"
#include "GameFramework/Actor.h"


const FIntVector FGridDir::FRONT =	FIntVector(1, 0, 0);
const FIntVector FGridDir::RIGHT =	FIntVector(0, 1, 0);
const FIntVector FGridDir::UP =		FIntVector(0, 0, 1);
const FIntVector FGridDir::BACK =	FIntVector(-1, 0, 0);
const FIntVector FGridDir::LEFT =	FIntVector(0, -1, 0);
const FIntVector FGridDir::DOWN =	FIntVector(0, 0, -1);

//FBlock
bool FBlock::ApplyDamage(int32 damageAmount){
	durability -= damageAmount;

	return durability < 1;
}

void FBlock::SetResource(const FResource * _resource){
	resource = _resource;
	
	durability = _resource->hardness;
	data.bIsSolid = _resource->id;
	data.color = _resource->color.ToFColor(true);

}

void FBlock::SetNoise(float newNoise) {
	data.noiseValue = newNoise;
}

void FBlock::SetBiomeColor(const FLinearColor & biomeColor){
	data.color = biomeColor.ToFColor(true);
}

bool FBlock::IsValid() const{
	return resource;
}

bool FBlock::IsAir() const{
	return !resource || resource->id == 0;
}

FBlock FBlock::FromId(AActor * owner, int32 resourceId){
	FBlock block;
	block.SetResource(FResource::FromId(owner, resourceId));
	return block;
}

FString FBlock::ToString() const{

	FString resorceStr = resource ? resource->ToString() : "nullptr";
	return "FBlock{data:" + data.ToString() + ", resource:" + resorceStr + ", durability:" + FString::FromInt(durability) + "}";
}

//FResource
FResource * FResource::FromId(AActor * owner, int32 resourceId){
		
	if (owner) { 
		UMyGameInstance* gameInstance = Cast<UMyGameInstance>(owner->GetGameInstance());
		return gameInstance->GetResourceTable()->FindRow<FResource>(*FString::FromInt(resourceId), FString());
	} 

	return nullptr;
}

FString FResource::ToString() const{

	return "FResource{id:" + FString::FromInt(id) + ", name:" + name.ToString() + ", solidity:" + FString::FromInt(solidity) + ", hardness:" + FString::FromInt(hardness) + ", color:" + "}";
}


//FBlockData
FString FBlockData::ToString() const{

	return "FBlockData{bIsSolid:" + FString::FromInt(bIsSolid) + ", color:" + "}";
}


//FBiomeData
FBiomeData* FSBiome::GRAS = nullptr;
FBiomeData* FSBiome::DESERT = nullptr;
FBiomeData* FSBiome::TUNDRA = nullptr;
FBiomeData* FSBiome::GRASDESERT = nullptr;
FBiomeData* FSBiome::SAVANNA = nullptr;
FBiomeData* FSBiome::WOODS = nullptr;
FBiomeData* FSBiome::TAIGA = nullptr;
FBiomeData* FSBiome::SEASONALFOREST = nullptr;
FBiomeData* FSBiome::FOREST = nullptr;
FBiomeData* FSBiome::RAINFOREST = nullptr;
FBiomeData* FSBiome::SWAMP = nullptr;
FBiomeData* FSBiome::STONE = nullptr;
FBiomeData* FSBiome::OCEAN = nullptr;


void FSBiome::InitStaticMembers(UMyGameInstance * gameInstance){
	
	UDataTable* biomeTable = gameInstance->GetBiomeTable();
	GRAS = biomeTable->FindRow<FBiomeData>("0", FString());
	DESERT = biomeTable->FindRow<FBiomeData>("1", FString());
	TUNDRA = biomeTable->FindRow<FBiomeData>("3", FString());
	GRASDESERT = biomeTable->FindRow<FBiomeData>("4", FString());
	SAVANNA = biomeTable->FindRow<FBiomeData>("5", FString());
	WOODS = biomeTable->FindRow<FBiomeData>("6", FString());
	TAIGA = biomeTable->FindRow<FBiomeData>("7", FString());
	SEASONALFOREST = biomeTable->FindRow<FBiomeData>("8", FString());
	FOREST = biomeTable->FindRow<FBiomeData>("9", FString());
	RAINFOREST = biomeTable->FindRow<FBiomeData>("10", FString());
	SWAMP = biomeTable->FindRow<FBiomeData>("11", FString());
	STONE = biomeTable->FindRow<FBiomeData>("12", FString());
	OCEAN = biomeTable->FindRow<FBiomeData>("13", FString());
}

FResource* FSResource::AIR = nullptr;
FResource* FSResource::DIRT = nullptr;
FResource* FSResource::GRASS = nullptr;
FResource* FSResource::STONE = nullptr;
FResource* FSResource::EARTHCORE = nullptr;
FResource* FSResource::COAL = nullptr;
FResource* FSResource::IRON = nullptr;
FResource* FSResource::GOLD = nullptr;
FResource* FSResource::WATER = nullptr;
FResource* FSResource::SAND = nullptr;
FResource* FSResource::SNOW = nullptr;

void FSResource::InitStaticMembers(UMyGameInstance * gameInstance) {

	UDataTable* resourceTable = gameInstance->GetResourceTable();
	AIR = resourceTable->FindRow<FResource>("0", FString());
	DIRT = resourceTable->FindRow<FResource>("1", FString());
	GRASS = resourceTable->FindRow<FResource>("2", FString());
	STONE = resourceTable->FindRow<FResource>("3", FString());
	EARTHCORE = resourceTable->FindRow<FResource>("4", FString());
	COAL = resourceTable->FindRow<FResource>("5", FString());
	IRON = resourceTable->FindRow<FResource>("6", FString());
	GOLD = resourceTable->FindRow<FResource>("7", FString());
	WATER = resourceTable->FindRow<FResource>("8", FString());
	SAND = resourceTable->FindRow<FResource>("9", FString());
	SNOW = resourceTable->FindRow<FResource>("10", FString());
}

FBlockGrid::FBlockGrid(){
}

FBlockGrid::FBlockGrid(int32 xSize, int32 ySize, int32 zSize){
	Init(xSize, ySize, zSize);
}

//FBlockGrid
void FBlockGrid::Init(int32 xSize, int32 ySize, int32 zSize){
	dims = FIntVector(xSize, ySize, zSize);
	grid.Init(TArray<TArray<FBlock>>(), xSize);

	for (size_t x = 0; x < xSize; x++) {
		grid[x].Init(TArray<FBlock>(), ySize);

		for (size_t y = 0; y < ySize; y++) {
			grid[x][y].Init(FBlock(), zSize);

		}
	}
}

bool FBlockGrid::IsValidIndex(int32 xIndex) const{
	return grid.IsValidIndex(xIndex);
}

bool FBlockGrid::IsValidIndex(int32 x, int32 y, int32 z) const{
	return grid.IsValidIndex(x) && grid[x].IsValidIndex(y) && grid[x][y].IsValidIndex(z);
}

bool FBlockGrid::IsValidIndex(const FIntVector & Index3D) const
{
	return IsValidIndex(Index3D.X, Index3D.Y, Index3D.Z);
}

const FBlock * FBlockGrid::GetBlockSafe(const FIntVector & Index3D) const{

	if (IsValidIndex(Index3D)) {
		return &grid[Index3D.X][Index3D.Y][Index3D.Z];
	}

	return nullptr;
}

TArray<TArray<FBlock>>& FBlockGrid::operator[](int32 xIndex){
	return grid[xIndex];
}

const TArray<TArray<FBlock>>& FBlockGrid::operator[](int32 xIndex) const{
	return grid[xIndex];
}


