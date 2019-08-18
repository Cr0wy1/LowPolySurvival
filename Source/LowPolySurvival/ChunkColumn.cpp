// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkColumn.h"
#include "MyGameInstance.h"
#include "WorldGenerator.h"

// Sets default values
UChunkColumn::UChunkColumn(){

}

void UChunkColumn::FinishDestroy(){
	//UE_LOG(LogTemp, Warning, TEXT("FinishDestroy: %s"), *chunkColLoc.ToString());

	Super::FinishDestroy();
	
}

void UChunkColumn::Init(AWorldGenerator * _worldGenerator){
	worldGenerator = _worldGenerator;
}


void UChunkColumn::InitTerrainNoiseMaps(){

	size_t size = FWorldParams::chunkSize;
	
	FIntVector blockLoc = ChunkToBlockLocation(chunkColLoc);

	hillsNoiseMap.Init(TArray<float>(), size);
	terrainNoiseMap.Init(TArray<float>(), size);
	heatNoiseMap.Init(TArray<float>(), size);
	rainNoiseMap.Init(TArray<float>(), size);

	for (int32 x = 0; x < size; x++) {
		hillsNoiseMap[x].Init(float(), size);
		terrainNoiseMap[x].Init(float(), size);
		heatNoiseMap[x].Init(float(), size);
		rainNoiseMap[x].Init(float(), size);

		for (int32 y = 0; y < size; y++) { 
			hillsNoiseMap[x][y] = Noise(blockLoc.X + x, blockLoc.Y + y, 1, 3, 128, 0.5);
			terrainNoiseMap[x][y] = worldGenerator->TerrainNoise(FVector2D(blockLoc.X + x, blockLoc.Y + y));
			heatNoiseMap[x][y] = worldGenerator->HeatNoise(FVector2D(blockLoc.X + x, blockLoc.Y + y));
			rainNoiseMap[x][y] = worldGenerator->RainNoise(FVector2D(blockLoc.X + x, blockLoc.Y + y));

		}
	}

	float A = 0.0f;
	float B = 1.0f; 
	 
	for (int32 x = 0; x < size; x++) {
		for (int32 y = 0; y < size; y++) {

			float test = FMath::Lerp(terrainNoiseMap[x][y], hillsNoiseMap[x][y] * 3.0f, rainNoiseMap[x][y]);

			terrainNoiseMap[x][y] = test;//worldGenerator->TerrainNoise(FVector2D(blockLoc.X + x, blockLoc.Y + y));

		}
	}


}

UChunkColumn * UChunkColumn::Construct(AWorldGenerator * _worldGenerator, FIntVector _chunkColLoc){
	_chunkColLoc.Z = 0;

	UChunkColumn* newChunkColumn = NewObject<UChunkColumn>();
	newChunkColumn->Init(_worldGenerator);
	newChunkColumn->Load(_chunkColLoc);

	FWorldLoader::loadedChunkColumns.Add(_chunkColLoc, newChunkColumn);

	return newChunkColumn;
}

void UChunkColumn::Load(FIntVector _chunkColLoc){
	chunkColLoc = _chunkColLoc;
	chunkColLoc.Z = 0;

	Create();
}

void UChunkColumn::Unload(){

	
}

void UChunkColumn::Create() {

	InitTerrainNoiseMaps();
}

const TArray<TArray<float>>* UChunkColumn::GetTerrainNoiseMap() const{
	return &terrainNoiseMap;
}

const TArray<TArray<float>>* UChunkColumn::GetHeatNoiseMap() const
{
	return &heatNoiseMap;
}

const TArray<TArray<float>>* UChunkColumn::GetRainNoiseMap() const
{
	return &rainNoiseMap;
}

