// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "Island.h"
#include "MyGameInstance.h"
#include "Engine/World.h"

// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AChunk::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChunk::Create(FVector2D _chunkLoc){
	chunkLoc = _chunkLoc;

	float baseSpawnChance = 0.2f;

	UDataTable* islandTable = GetGameInstance<UMyGameInstance>()->GetIslandTable();

	TArray<FIslandInfo*> islandInfos;
	islandTable->GetAllRows(FString(), islandInfos);

	float spawnChance = (FMath::Rand() % 100) * 0.01;
	
	if (spawnChance <= baseSpawnChance) {
		float zLoc = ((FMath::Rand() % 1000) - 500) * 100;
		float zRot = FMath::Rand() % 360;

		GetWorld()->SpawnActor<AIsland>(islandInfos[0]->island_BP, FVector(chunkLoc * 10000, zLoc), FRotator(0, zRot, 0));
	}

	
}

void AChunk::Load(FVector2D _chunkLoc){
	chunkLoc = _chunkLoc;
}

void AChunk::Unload(){
	Destroy();
}

