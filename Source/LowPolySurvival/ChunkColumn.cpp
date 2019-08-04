// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkColumn.h"
#include "MyGameInstance.h"
#include "WorldGenerator.h"

// Sets default values
UChunkColumn::UChunkColumn(){

}

void UChunkColumn::Init(AWorldGenerator * _worldGenerator){
	worldGenerator = _worldGenerator;
}


void UChunkColumn::InitTerrainNoiseMap(){

	size_t size = FWorldParams::chunkSize;
	
	FIntVector blockLoc = ChunkToBlockLocation(chunkColLoc);

	terrainNoiseMap.Init(TArray<float>(), size);
	for (int32 x = 0; x < size; x++) {
		terrainNoiseMap[x].Init(float(), size);
		for (int32 y = 0; y < size; y++) { 
			terrainNoiseMap[x][y] = worldGenerator->TerrainNoise(FVector2D(blockLoc.X + x, blockLoc.Y + y));

		}
	}


}

void UChunkColumn::Create(FIntVector _chunkColLoc) {
	chunkColLoc = _chunkColLoc;
	chunkColLoc.Z = 0;

	InitTerrainNoiseMap();
}

const TArray<TArray<float>>* UChunkColumn::GetTerrainNoiseMap() const{
	return &terrainNoiseMap;
}

