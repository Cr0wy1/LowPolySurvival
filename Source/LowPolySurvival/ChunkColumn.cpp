// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkColumn.h"
#include "MyGameInstance.h"
#include "WorldGenerator.h"

// Sets default values
AChunkColumn::AChunkColumn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComponent;
}

void AChunkColumn::Init(AWorldGenerator * _worldGenerator){
	worldGenerator = _worldGenerator;
}

// Called when the game starts or when spawned
void AChunkColumn::BeginPlay(){
	Super::BeginPlay();
	
}

// Called every frame
void AChunkColumn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChunkColumn::InitTerrainNoiseMap(){

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

void AChunkColumn::Create(FIntVector _chunkColLoc) {
	chunkColLoc = _chunkColLoc;
	chunkColLoc.Z = 0;

	InitTerrainNoiseMap();
}

const TArray<TArray<float>>* AChunkColumn::GetTerrainNoiseMap() const{
	return &terrainNoiseMap;
}

