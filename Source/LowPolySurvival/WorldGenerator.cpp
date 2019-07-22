// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGenerator.h"
#include "Engine/World.h"
#include "Chunk.h"
#include "PlayercharController.h"
#include "DrawDebugHelpers.h"
#include "MyGameInstance.h"
#include "ChunkColumn.h"
#include "Queue.h"

 
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
		FIntVector chunkLoc = WorldToChunkLocation(playerPos);

		if (chunkLoc != cPlayerChunkLoc) { 
			cPlayerChunkLoc = chunkLoc;
			OnEnterChunk();
			//UE_LOG(LogTemp, Warning, TEXT("World Generator( controllers: %s, playerLoc: %s, chunkLoc: %s )"), *playerController->GetName(), *playerPos.ToString(), *chunkLoc.ToString());
		}

	}

}

void AWorldGenerator::OnEnterChunk(){

	CheckChunks(cPlayerChunkLoc);
	
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

		FIntVector deltaRange = loadedChunk.Key - cPlayerChunkLoc;

		//Check Terrainmesh < checkedRadius for mesh connection
		if (deltaRange.X < checkedRadiusXY && deltaRange.Y < checkedRadiusXY && deltaRange.Z < checkedRadiusZ) {
			loadedChunk.Value->GenerateTerrainMesh();
		}
		
	}


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

	FIntVector chunkColLoc = chunkLoc;
	chunkColLoc.Z = 0;
	auto findChunkCol = loadedChunkColumns.Find(chunkColLoc);

	AChunkColumn* responseChunkColumn;

	if (!findChunkCol) {
		AChunkColumn* newChunkColumn = GetWorld()->SpawnActor<AChunkColumn>(AChunkColumn::StaticClass(), ChunkToWorldLocation(chunkColLoc), FRotator::ZeroRotator);
		newChunkColumn->Init(this);
		newChunkColumn->Create(chunkColLoc);

		loadedChunkColumns.Add(chunkColLoc, newChunkColumn);
		responseChunkColumn = newChunkColumn;
	}
	else {
		responseChunkColumn = *findChunkCol;
	}

	AChunk* newChunk = GetWorld()->SpawnActor<AChunk>(AChunk::StaticClass(), ChunkToWorldLocation(chunkLoc), FRotator::ZeroRotator);
	newChunk->Init(this, responseChunkColumn);
	
	loadedChunks.Add(chunkLoc, newChunk);

	if (createdChunks.Contains(chunkLoc)) {
		newChunk->Load(chunkLoc);
	}
	else {
		createdChunks.Add(chunkLoc);
		newChunk->Create(chunkLoc);
	}

	newChunk->SetTerrainMaterial(terrainMaterial);

}

void AWorldGenerator::CheckChunks(FIntVector center){

	int32 radiusXY = checkedRadiusXY * 2 + 1;
	int32 radiusZ = checkedRadiusZ * 2 + 1;

	int32 dz = 0;
	int32 zDir = -1;
	for (int32 z = 0; z < radiusZ; ++z){

		dz += (z*zDir);
		zDir *= -1;

		int32 x, y, dx, dy;
		x = y = dx = 0;
		dy = -1;
		
		int32 dim = radiusXY;
		int32 maxI = dim * dim;
		for (int32 i = 0; i < maxI; i++) {

			FIntVector chunkLoc(center.X + x, center.Y + y, center.Z + dz);
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

	
}

void AWorldGenerator::PlaceBlock(FIntVector blockLocation, const FBlockData & blockData){

	//TODO maybe Refactor
	TArray<FIntVector> chunkBlockLocs;
	TArray<FIntVector> chunkLocs = BlockToChunkBlockLocation(blockLocation, chunkBlockLocs);

	loadedChunks[chunkLocs[0]]->SetBlock(chunkBlockLocs[0], blockData);

	for (size_t i = 1; i < chunkLocs.Num(); i++) {
		loadedChunks[chunkLocs[i]]->UpdateTerrainMesh(chunkBlockLocs[i]);

	}

}

void AWorldGenerator::RemoveBlock(FIntVector blockLocation){

	PlaceBlock(blockLocation, FBlockData(0));
}



float AWorldGenerator::TerrainNoise(const FVector2D &loc) const{

	//with Octaves
	float noise = Noise(loc.X, loc.Y, noiseParams);

	//Apply Redistribution
	noise = FMath::Pow(noise, noiseParams.redistribution);


	//Apply Terraces
	//uint32 terracesSteps = 2;
	//noise = FMath::RoundToFloat(noise * terracesSteps) / terracesSteps;

	return noise;
}

float AWorldGenerator::CaveNoise(const FVector & loc) const{

	float noise = Noise3D(loc.X, loc.Y, loc.Z, caveNoiseParams);
	
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

int32 AWorldGenerator::GetLoadedChunksNum() const
{
	
	return loadedChunks.Num();
}

bool AWorldGenerator::IsChunkLoaded(const FIntVector & chunkLoc) const{
	return loadedChunks.Contains(chunkLoc);
}


