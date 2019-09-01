// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGenerator.h"
#include "Engine/World.h"
#include "Chunk.h"
#include "PlayercharController.h"
#include "DrawDebugHelpers.h"
#include "MyGameInstance.h"
#include "ChunkColumn.h"
#include "Queue.h"
#include "FunctionLibrary.h"
#include "ChunkLoader.h"



TMap<FIntVector, UChunkColumn*> FWorldLoader::loadedChunkColumns = TMap<FIntVector, UChunkColumn*>();
TMap<FIntVector, AChunk*> FWorldLoader::loadedChunks = TMap<FIntVector, AChunk*>();

 
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

	gameInstance = GetGameInstance<UMyGameInstance>();
	playerController = GetWorld()->GetFirstPlayerController<APlayercharController>();
	
	chunkLoader = UChunkLoader::Construct(this, checkedRadiusXY, checkedRadiusZ);

	//Random Seed
	FMath::RandInit(0);

	CleanAllChunks();

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
			chunkLoader->UpdateLocation(cPlayerChunkLoc);

			//UE_LOG(LogTemp, Warning, TEXT("World Generator( controllers: %s, playerLoc: %s, chunkLoc: %s )"), *playerController->GetName(), *playerPos.ToString(), *chunkLoc.ToString());
		}

	}

}

void AWorldGenerator::CleanAllChunks(){
	
	//if (FWorldLoader::loadedChunks.Num() > 0){
	//	for (auto loadedChunk : FWorldLoader::loadedChunks) {
	//		if (loadedChunk.Value) {
	//			loadedChunk.Value->Unload();
	//		}

	//	}
	//}


	FWorldLoader::loadedChunkColumns.Reset();
	FWorldLoader::loadedChunks.Reset();
	checkedChunkLocs.Reset();
}



void AWorldGenerator::HitBlock(FIntVector blockLocation, float damageAmount, AActor* causer){
	FIntVector chunkBlockLoc;
	FIntVector chunkLoc = BlockToChunkBlockLocation(blockLocation, chunkBlockLoc);

	
	FWorldLoader::loadedChunks[chunkLoc]->HitBlock(chunkBlockLoc, damageAmount, causer);
	
}

void AWorldGenerator::RemoveBlock(FIntVector blockLocation){

	SetBlock(blockLocation, FBlock::FromId(this, 0));
}



float AWorldGenerator::BiomeNoise(const FVector2D & loc) const{

	//with Octaves
	float noise = Noise(loc.X, loc.Y, biomeNoiseParams);

	//Apply Redistribution
	noise = FMath::Pow(noise, biomeNoiseParams.redistribution);


	//Apply Terraces
	//uint32 terracesSteps = 2;
	//noise = FMath::RoundToFloat(noise * terracesSteps) / terracesSteps;

	return noise;
}

float AWorldGenerator::TerrainNoise(const FVector2D &loc) const {

	float totalNoise = 0.0f;

	for (FNoiseParams params : noiseParams) {

		//with Octaves
		float noise = Noise(loc.X, loc.Y, params);

		//Apply Redistribution
		noise = FMath::Pow(noise, params.redistribution);


		//Apply Terraces
		//uint32 terracesSteps = 2; 
		//noise = FMath::RoundToFloat(noise * terracesSteps) / terracesSteps;

		totalNoise += noise;
	}

	return totalNoise;
}

float AWorldGenerator::CaveNoise(const FVector & loc) const{

	float noise = Noise3D(loc.X, loc.Y, loc.Z, caveNoiseParams);
	
	return noise;
}

float AWorldGenerator::OreNoise(const FVector & loc) const{
	float noise = Noise3D(loc.X * 2, loc.Y * 2, loc.Z * 2, oreNoiseParams);

	return noise;
}

float AWorldGenerator::HeatNoise(const FVector2D & loc) const{
	return Noise(loc.X*0.5, loc.Y*0.5, 6, 0.5, 32);
}

float AWorldGenerator::RainNoise(const FVector2D & loc) const
{
	return Noise(loc.X*0.5, loc.Y*0.5, 6, 0.5, 32);
}

const TArray<FNoiseParams> AWorldGenerator::GetNoiseParams() const{
	return noiseParams;
}

const FGenerationParams AWorldGenerator::GetGenerationParams() const{
	return generationParams;
}

bool AWorldGenerator::SetBlock(const FBlockLoc & blockLoc, const FBlock & block){

	bool bIsSetted = false;
	//TODO maybe Refactor
	TArray<FIntVector> chunkBlockLocs;
	TArray<FIntVector> chunkLocs = BlockToChunkBlockLocation(blockLoc, chunkBlockLocs);

	AChunk** chunk = FWorldLoader::loadedChunks.Find(chunkLocs[0]);

	if (chunk) {
		bIsSetted = (*chunk)->SetBlock(chunkBlockLocs[0], block);
		
		//Update surrounding Chunks, maybe refactor
		for (size_t i = 1; i < chunkLocs.Num(); i++) {

			AChunk** chunkToUpdate = FWorldLoader::loadedChunks.Find(chunkLocs[i]);

			if (chunkToUpdate) {
				(*chunkToUpdate)->UpdateTerrainMesh(chunkBlockLocs[i]);
			}
		}
	}

	return bIsSetted;
}

const FBlock * AWorldGenerator::GetBlock(const FBlockLoc & blockLoc){

	FIntVector chunkBlockLoc;
	FIntVector chunkLoc = BlockToChunkBlockLocation(blockLoc, chunkBlockLoc);

	AChunk** chunk = FWorldLoader::loadedChunks.Find(chunkBlockLoc);
	
	if (chunk) {
		return (*chunk)->GetBlock(chunkBlockLoc);
	}

	return nullptr;
}

FBiomeData* AWorldGenerator::GetBiome(float heatNoise, float rainNoise) const{

	if (rainNoise < 0.25f) {
		if (heatNoise > 0.65f) {
			return FBiomeData::DATA::DESERT;
		}else if (heatNoise > 0.25f){
			return FBiomeData::DATA::GRASDESERT;
		}
		else {
			return FBiomeData::DATA::TUNDRA;
		}
	}
	else if (rainNoise < 0.5f) {
		if (heatNoise > 0.75f) {
			return FBiomeData::DATA::SAVANNA;
		}
		else if (heatNoise > 0.50f) {
			return FBiomeData::DATA::WOODS;
		}
		else if (heatNoise > 0.25f) {
			return FBiomeData::DATA::TAIGA;
		}
		else {
			return FBiomeData::DATA::TUNDRA;
		}
	}
	else if (rainNoise < 0.75f) {
		if (heatNoise > 0.75f) {
			return FBiomeData::DATA::SEASONALFOREST;
		}
		else if (heatNoise > 0.50f) {
			return FBiomeData::DATA::FOREST;
		}
		else if (heatNoise > 0.25f) {
			return FBiomeData::DATA::TAIGA;
		}
		else {
			return FBiomeData::DATA::TUNDRA;
		}
	}else{
		if (heatNoise > 0.75f) {
			return FBiomeData::DATA::RAINFOREST;
		}
		else if (heatNoise > 0.50f) {
			return FBiomeData::DATA::SWAMP;
		}
		else if (heatNoise > 0.25f) {
			return FBiomeData::DATA::TAIGA;
		}
		else {
			return FBiomeData::DATA::TUNDRA;
		}
		
	}

}

FBiomeData * AWorldGenerator::GetBiome(const FBlockLoc & blockLoc) const{

	FIntVector chunkBlockLoc;
	FIntVector chunkLoc = BlockToChunkBlockLocation(blockLoc, chunkBlockLoc);
	chunkLoc.Z = 0;

	UChunkColumn*const* responseChunkCol = FWorldLoader::loadedChunkColumns.Find(chunkLoc);

	//UE_LOG(LogTemp, Warning, TEXT("chunkBockLoc: %s"), *chunkLoc.ToString());


	if (responseChunkCol) {
		//UE_LOG(LogTemp, Warning, TEXT("test"));

		auto heatNoiseMap = (*(*responseChunkCol)->GetHeatNoiseMap())[chunkBlockLoc.X][chunkBlockLoc.Y]; 
		auto rainNoiseMap = (*(*responseChunkCol)->GetRainNoiseMap())[chunkBlockLoc.X][chunkBlockLoc.Y]; 
		
		return GetBiome(heatNoiseMap, rainNoiseMap);
	}
	 
	return nullptr;
}

AChunk * AWorldGenerator::GetChunk(const FChunkLoc & chunkLoc) const{
	return FWorldLoader::loadedChunks[chunkLoc];
}

AChunk * AWorldGenerator::GetChunkFromBlock(const FBlockLoc & blockLoc) const{
	FChunkLoc chunkLoc(blockLoc.X >> 4, blockLoc.Y >> 4, blockLoc.Z >> 4);
	return GetChunk(chunkLoc);
}

AChunk * AWorldGenerator::GetChunkSafe(const FIntVector & chunkLoc) const{
	AChunk *const* findChunk = FWorldLoader::loadedChunks.Find(chunkLoc);
	return findChunk ? *findChunk : nullptr;
}

int32 AWorldGenerator::GetLoadedChunksNum() const
{
	
	return FWorldLoader::loadedChunks.Num();
}

FString AWorldGenerator::GetWorldName() const{
	return worldName;
}

bool AWorldGenerator::IsChunkLoaded(const FIntVector & chunkLoc) const{
	return FWorldLoader::loadedChunks.Contains(chunkLoc);
}


