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
	
	TArray<FIntVector> chunksToUnload;

	//collect unloaded chunks
	for (auto loadedChunk : loadedChunks){
		if (!checkedChunkLocs.Contains(loadedChunk.Key)) {
			chunksToUnload.Add(loadedChunk.Key);
		}
	}

	//Unload unloaded chunks
	for (const FIntVector loc : chunksToUnload) {
		loadedChunks[loc]->Unload();
		loadedChunks.Remove(loc);
		//UE_LOG(LogTemp, Warning, TEXT("removed"));
	}


	checkedChunkLocs.Empty();

	//Generate Terrain Mesh for loaded Chunks
	for (auto loadedChunk : loadedChunks) {
		loadedChunk.Value->GenerateTerrainMesh();
	}

	//UE_LOG(LogTemp, Warning, TEXT("WorldGen: TMap size: %i"), loadedChunks.Num());

}

void AWorldGenerator::OnCheckChunk(FIntVector chunkLoc){

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



void AWorldGenerator::LoadChunk(FIntVector chunkLoc){

	AChunk* newChunk = GetWorld()->SpawnActor<AChunk>(AChunk::StaticClass(), ChunkToWorldLocation(chunkLoc), FRotator::ZeroRotator);
	newChunk->Init(this);
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

	//UE_LOG(LogTemp, Warning, TEXT("CenterChunk: %s"), *(FVector(centerX, centerY, 0).ToString()));


	int32 x, y, dx, dy;
	x = y = dx = 0;
	dy = -1;
	int32 dim = checkedRadius * 2 + 1;
	int32 maxI = dim * dim;
	for (int32 i = 0; i < maxI; i++) {

		//UE_LOG(LogTemp, Warning, TEXT("Spiral:(x:%i, y:%i"), (centerX + x), (centerY + y));

		FIntVector chunkLoc(centerX + x, centerY + y, 0);
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

void AWorldGenerator::RemoveBlock(FIntVector blockLocation){

	TArray<FIntVector> chunkBlockLocs;
	TArray<FIntVector> chunkLocs = BlockToChunkBlockLocation(blockLocation, chunkBlockLocs);

	for (size_t i = 0; i < chunkLocs.Num(); i++){
		//UE_LOG(LogTemp, Warning, TEXT("Chunk Locations: %s"), *chunkLocs[i].ToString());

		loadedChunks[chunkLocs[i]]->RemoveBlock(chunkBlockLocs[i]);
	}

	//for (auto loadedChunk : loadedChunks) {
	//	UE_LOG(LogTemp, Warning, TEXT("loadedChunks: %s"), *loadedChunk.Key.ToString());

	//}
	 
}

float AWorldGenerator::BlockNoise(float blockX, float blockY) const{

	//with Octaves
	float noise = Noise(blockX, blockY, noiseParams);

	//Apply Redistribution
	noise = FMath::Pow(noise, noiseParams.redistribution);

	//Apply Terraces
	//uint32 terracesSteps = 2;
	//noise = FMath::RoundToFloat(noise * terracesSteps) / terracesSteps;


	return noise;
}

const FNoiseParams AWorldGenerator::GetNoiseParams() const{
	return noiseParams;
}

const FGenerationParams AWorldGenerator::GetGenerationParams() const{
	return generationParams;
}

AChunk * AWorldGenerator::GetChunk(const FIntVector & chunkLoc) const{
	return loadedChunks[chunkLoc];
}

AChunk * AWorldGenerator::GetChunkSafe(const FIntVector & chunkLoc) const{
	AChunk *const* findChunk = loadedChunks.Find(chunkLoc);
	return findChunk ? *findChunk : nullptr;
}

bool AWorldGenerator::IsChunkLoaded(const FIntVector & chunkLoc) const{
	return loadedChunks.Contains(chunkLoc);
}


