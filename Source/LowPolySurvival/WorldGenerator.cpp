// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGenerator.h"
#include "Engine/World.h"
#include "Chunk.h"
#include "PlayercharController.h"
#include "DrawDebugHelpers.h"
#include "MyGameInstance.h"


// Sets default values
AWorldGenerator::AWorldGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
}

// Called when the game starts or when spawned
void AWorldGenerator::BeginPlay()
{
	Super::BeginPlay();

	playerController = GetWorld()->GetFirstPlayerController<APlayercharController>();
	
	//Random Seed
	FMath::RandInit(0);


}
  

 
// Called every frame
void AWorldGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	  
	
	if (playerController) {
		FVector playerPos(playerController->GetPawn()->GetActorLocation()); 
		FVector2D chunkLoc = WorldToChunkLocation(playerPos); 

		if (chunkLoc != cPlayerChunkLoc) { 
			cPlayerChunkLoc = chunkLoc;
			OnEnterChunk();
			//UE_LOG(LogTemp, Warning, TEXT("World Generator( controllers: %s, playerLoc: %s, chunkLoc: %s )"), *playerController->GetName(), *playerPos.ToString(), *chunkLoc.ToString());
		}

	}

}

void AWorldGenerator::OnEnterChunk(){

	CheckChunks(cPlayerChunkLoc.X, cPlayerChunkLoc.Y);
	
	TArray<FVector2D> chunksToUnload;

	for (auto loadedChunk : loadedChunks){
		if (!checkedChunkLocs.Contains(loadedChunk.Key)) {
			chunksToUnload.Add(loadedChunk.Key);
		}
	}

	for (const FVector2D loc : chunksToUnload) {
		loadedChunks[loc]->Unload();
		loadedChunks.Remove(loc);
		//UE_LOG(LogTemp, Warning, TEXT("removed"));
	}


	checkedChunkLocs.Empty();

	//UE_LOG(LogTemp, Warning, TEXT("WorldGen: TMap size: %i"), loadedChunks.Num());

}

void AWorldGenerator::OnCheckChunk(FVector2D chunkLoc){

	checkedChunkLocs.Add(chunkLoc);

	if (!loadedChunks.Contains(chunkLoc)) {
		LoadChunk(chunkLoc);
	}

	if (bDrawDebug) {
		FVector startLoc = ChunkToWorldLocation(chunkLoc);
		FVector endLoc = startLoc + FVector(0, 0, 20000);
		DrawDebugLine(GetWorld(), startLoc, endLoc, FColor::Red, true, 1000, 0, 100);
	}

}


void AWorldGenerator::LoadChunk(FVector2D chunkLoc){

	AChunk* newChunk = GetWorld()->SpawnActor<AChunk>(AChunk::StaticClass(), ChunkToWorldLocation(chunkLoc), FRotator::ZeroRotator);
	//UE_LOG(LogTemp, Warning, TEXT("LoadChunk: chunk spawned at %s"), *FVector(chunkLoc * chunkSize, 0).ToString());

	
	loadedChunks.Add(chunkLoc, newChunk);

	if (createdChunks.Contains(chunkLoc)) {
		newChunk->Load(chunkLoc);
	}
	else {
		createdChunks.Add(chunkLoc);
		newChunk->Create(chunkLoc);
	}

	newChunk->SetTerrainMaterial(terrainMaterial);

	//UE_LOG(LogTemp, Warning, TEXT("WorldGenerator: load Chunk at: %s"), *chunkLoc.ToString());
}

void AWorldGenerator::CheckChunks(int32 centerX, int32 centerY){

	int32 x, y, dx, dy;
	x = y = dx = 0;
	dy = -1;
	int32 dim = checkedRadius * 2 + 1;
	int32 maxI = dim * dim;
	for (int32 i = 0; i < maxI; i++) {

		//UE_LOG(LogTemp, Warning, TEXT("Spiral:(x:%i, y:%i"), (centerX + x), (centerY + y));

		FVector2D chunkLoc(centerX + x, centerY + y);
		OnCheckChunk(chunkLoc);

		if ((x == y) || ((x < 0) && (x == -y)) || ((x > 0) && (x == 1 - y))) {
			dim = dx;
			dx = -dy;
			dy = dim;
		}
		x += dx;
		y += dy;
	}
	
}

