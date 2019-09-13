// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkColumn.h"
#include "MyGameInstance.h"
#include "WorldGenerator.h"



// Sets default values
UChunkColumn::UChunkColumn(){
	
	//AddToRoot();

}

void UChunkColumn::FinishDestroy(){
	//UE_LOG(LogTemp, Warning, TEXT("FinishDestroy: %s"), *chunkColLoc.ToString());

	Unload();
	
	UE_LOG(LogTemp, Warning, TEXT("UChunkColumn::FinishDestroy chunkLoc: %s"), *chunkColLoc.ToString());

	Super::FinishDestroy();
}

void UChunkColumn::Init(AWorldGenerator * _worldGenerator){
	worldGenerator = _worldGenerator;
}


void UChunkColumn::InitTerrainNoiseMaps(){

	size_t size = FWorldParams::chunkSize;
	
	FIntVector blockLoc = ChunkToBlockLocation(chunkColLoc);

	hillsNoiseMap.Init(TArray<float>(), size);
	//terrainNoiseMap.Init(TArray<float>(), size);

	for (int32 x = 0; x < size; x++) {
		hillsNoiseMap[x].Init(float(), size);
		//terrainNoiseMap[x].Init(float(), size);

		for (int32 y = 0; y < size; y++) { 
			hillsNoiseMap[x][y] = FMath::Pow( Noise(blockLoc.X + x + 1000, blockLoc.Y + y + 1000, 2, 2), 2) * 0.2;
			//terrainNoiseMap[x][y] = worldGenerator->TerrainNoise(FVector2D(blockLoc.X + x, blockLoc.Y + y)) * 0.1;

		}
	}

	USimplexNoise::NoiseOctaves(terrainNoiseMap, blockLoc.X, blockLoc.Y, size, size, 4, 0.5, 0.5);
	USimplexNoise::setNoiseSeed(worldGenerator->GetSeed() + 1000 );
	USimplexNoise::NoiseOctaves(heatNoiseMap, blockLoc.X, blockLoc.Y, size, size, 6, 0.5, 0.5);
	USimplexNoise::setNoiseSeed(worldGenerator->GetSeed() + 2000); 
	USimplexNoise::NoiseOctaves(rainNoiseMap, blockLoc.X, blockLoc.Y, size, size, 6, 0.5, 0.5);
	USimplexNoise::setNoiseSeed(worldGenerator->GetSeed() + 3000);
	USimplexNoise::NoiseOctaves(hillsNoiseMap, blockLoc.X, blockLoc.Y, size, size, 2, 2, 0.5);

	for (int32 x = 0; x < size; x++) {
		for (int32 y = 0; y < size; y++) { 

			terrainNoiseMap[x][y] = terrainNoiseMap[x][y] * 0.3f;
			//terrainNoiseMap[x][y] = FMath::Pow(terrainNoiseMap[x][y], 3) * 0.2f;
			hillsNoiseMap[x][y] = FMath::Pow(terrainNoiseMap[x][y], 6);

			//terrainNoiseMap[x][y] = FMath::Lerp(terrainNoiseMap[x][y], hillsNoiseMap[x][y], 0.5f);
			terrainNoiseMap[x][y] += hillsNoiseMap[x][y];
			 
			if (terrainNoiseMap[x][y] < 0.2f) { 
				//terrainNoiseMap[x][y] = 0.19f;
			 } 

			//terrainNoiseMap[x][y] *= 0.3f;
			//terrainNoiseMap[x][y] = finalNoise;//worldGenerator->TerrainNoise(FVector2D(blockLoc.X + x, blockLoc.Y + y));

		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("UChunkColumn::InitTerrainNoiseMaps chunkLoc: %s"), *chunkColLoc.ToString());
}

UChunkColumn * UChunkColumn::Construct(AWorldGenerator * _worldGenerator, FIntVector _chunkColLoc){
	_chunkColLoc.Z = 0;

	UChunkColumn* newChunkColumn = NewObject<UChunkColumn>();
	newChunkColumn->Init(_worldGenerator);
	newChunkColumn->Load(_chunkColLoc);

	AWorldGenerator::loader.loadedChunkColumns.Add(_chunkColLoc, newChunkColumn);

	//UE_LOG(LogTemp, Warning, TEXT("UChunkColumn::Construct chunkLoc: %s"), *_chunkColLoc.ToString());

	return newChunkColumn;
}

void UChunkColumn::Load(FIntVector _chunkColLoc){
	chunkColLoc = _chunkColLoc;
	chunkColLoc.Z = 0;

	Create();
}

void UChunkColumn::Unload(){

	AWorldGenerator::loader.loadedChunkColumns.Remove(chunkColLoc);
	
}

void UChunkColumn::AddChunk(){
	chunksHolding++;
}

void UChunkColumn::RemoveChunk(){
	chunksHolding--;
	if (chunksHolding <= 0) {
		//RemoveFromRoot();
	}
}

void UChunkColumn::Create() {

	InitTerrainNoiseMaps();
}


