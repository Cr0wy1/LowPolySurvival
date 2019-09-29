// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGenerator.h"
#include "Engine/World.h"
#include "Chunk.h"
#include "MyGameInstance.h"
#include "ChunkColumn.h"
#include "BiomeGenLibrary.h"

FWorldLoader AWorldGenerator::loader = FWorldLoader();

//TMap<FIntVector, UChunkColumn*> FWorldLoader::loadedChunkColumns = TMap<FIntVector, UChunkColumn*>();
//TMap<FIntVector, AChunk*> FWorldLoader::loadedChunks = TMap<FIntVector, AChunk*>();

 
// Sets default values
AWorldGenerator::AWorldGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	
}

// Called when the game starts or when spawned
void AWorldGenerator::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("AWorldGenerator beginPlayer before Clean: %s"), *loader.ToString());

	 
	//Random Seed
	FMath::RandInit(seed);

	CleanAllChunks();

	UE_LOG(LogTemp, Warning, TEXT("AWorldGenerator beginPlayer after Clean: %s"), *loader.ToString());

}
   

 
// Called every frame 
void AWorldGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AWorldGenerator::CleanAllChunks(){
	
	//if (FWorldLoader::loadedChunks.Num() > 0){
	//	for (auto loadedChunk : FWorldLoader::loadedChunks) {
	//		if (loadedChunk.Value) {
	//			loadedChunk.Value->Unload();
	//		}

	//	}
	//}


	AWorldGenerator::loader.loadedChunkColumns.Reset();
	AWorldGenerator::loader.loadedChunks.Reset();
	checkedChunkLocs.Reset();
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

bool AWorldGenerator::HitBlock(FIntVector blockLocation, float damageAmount, AActor* causer) {
	bool bIsDestroyed = false;
	//TODO maybe Refactor
	TArray<FIntVector> chunkBlockLocs;
	TArray<FIntVector> chunkLocs = BlockToChunkBlockLocation(blockLocation, chunkBlockLocs);

	AChunk** chunk = loader.loadedChunks.Find(chunkLocs[0]);
	if (chunk) {
		bIsDestroyed = (*chunk)->HitBlock(chunkBlockLocs[0], damageAmount, causer);

		if (bIsDestroyed) {
			//Update surrounding Chunks, maybe refactor
			for (size_t i = 1; i < chunkLocs.Num(); i++) {
				UpdateChunk(chunkLocs[i], chunkBlockLocs[i]);
			}
		}

	}
	return bIsDestroyed;
}

void AWorldGenerator::RemoveBlock(FIntVector blockLocation) {

	SetBlock(blockLocation, FBlock::FromId(this, 0));
}

bool AWorldGenerator::SetBlock(const FBlockLoc & blockLoc, const FBlock & block){

	bool bIsSetted = false;
	//TODO maybe Refactor
	TArray<FIntVector> chunkBlockLocs;
	TArray<FIntVector> chunkLocs = BlockToChunkBlockLocation(blockLoc, chunkBlockLocs);

	AChunk** chunk = loader.loadedChunks.Find(chunkLocs[0]);

	if (chunk) {
		bIsSetted = (*chunk)->SetBlock(chunkBlockLocs[0], block);
		
		//Update surrounding Chunks, maybe refactor
		for (size_t i = 1; i < chunkLocs.Num(); i++) {
			UpdateChunk(chunkLocs[i], chunkBlockLocs[i]);
		}
	}

	return bIsSetted;
}

const FBlock * AWorldGenerator::GetBlock(const FBlockLoc & blockLoc){

	FIntVector chunkBlockLoc;
	FIntVector chunkLoc = BlockToChunkBlockLocation(blockLoc, chunkBlockLoc);

	AChunk** chunk = loader.loadedChunks.Find(chunkBlockLoc);
	
	if (chunk) {
		return (*chunk)->GetBlock(chunkBlockLoc);
	}

	return nullptr;
}

void AWorldGenerator::UpdateChunk(const FChunkLoc & chunkLoc){
	AChunk** chunk = loader.loadedChunks.Find(chunkLoc);
	if (chunk) {
		(*chunk)->UpdateTerrainMesh();
	}
}

void AWorldGenerator::UpdateChunk(const FChunkLoc & chunkLoc, const FIntVector & chunkBlockLoc){
	AChunk** chunk = loader.loadedChunks.Find(chunkLoc);
	if (chunk) {
		(*chunk)->UpdateTerrainMesh(chunkBlockLoc);
	}
}

FBiomeData* AWorldGenerator::GetBiome(float xNoise, float yNoise) const{
	
	return FSBiomeDiagram::baseBiomeDiagram.GetBiome(xNoise, yNoise);

}

FBiomeData * AWorldGenerator::GetBiome(const FBlockLoc & blockLoc) const{

	FIntVector chunkBlockLoc;
	FIntVector chunkLoc = BlockToChunkBlockLocation(blockLoc, chunkBlockLoc);
	chunkLoc.Z = 0;

	UChunkColumn*const* responseChunkCol = loader.loadedChunkColumns.Find(chunkLoc);

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
	return loader.loadedChunks[chunkLoc];
}

AChunk * AWorldGenerator::GetChunk(const FVector & worldLoc) const{
	return loader.loadedChunks[WorldToChunkLocation(worldLoc)];
}

AChunk * AWorldGenerator::GetChunkFromBlock(const FBlockLoc & blockLoc) const{
	FChunkLoc chunkLoc(blockLoc.X >> 4, blockLoc.Y >> 4, blockLoc.Z >> 4);
	return GetChunk(chunkLoc);
}

AChunk * AWorldGenerator::GetChunkSafe(const FIntVector & chunkLoc) const{
	AChunk *const* findChunk = loader.loadedChunks.Find(chunkLoc);
	return findChunk ? *findChunk : nullptr;
}

AChunk * AWorldGenerator::GetChunkSafe(const FVector & worldLoc) const{
	AChunk *const* findChunk = loader.loadedChunks.Find(WorldToChunkLocation(worldLoc));
	return findChunk ? *findChunk : nullptr;
}

int32 AWorldGenerator::GetLoadedChunksNum() const
{
	
	return loader.loadedChunks.Num();
}

FString AWorldGenerator::GetWorldName() const{
	return worldName;
}

int32 AWorldGenerator::GetSeed() const
{
	return seed;
}

bool AWorldGenerator::IsChunkLoaded(const FIntVector & chunkLoc) const{
	return loader.loadedChunks.Contains(chunkLoc);
}


