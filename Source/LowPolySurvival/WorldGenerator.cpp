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
	
	//Random Seed
	FMath::RandInit(0);

	FWorldLoader::loadedChunkColumns.Reset();
	FWorldLoader::loadedChunks.Reset();


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
	for (auto loadedChunk : FWorldLoader::loadedChunks){
		if (!checkedChunkLocs.Contains(loadedChunk.Key)) {
			chunksToUnload.Add(loadedChunk.Key);
		}
	}

	//Unload unloaded chunks
	for (const FIntVector loc : chunksToUnload) {
		
		FWorldLoader::loadedChunks[loc]->Unload();
		FWorldLoader::loadedChunks.Remove(loc);
		//UE_LOG(LogTemp, Warning, TEXT("removed"));
	} 


	checkedChunkLocs.Empty();

	//Generate Terrain Mesh for loaded Chunks
	for (auto loadedChunk : FWorldLoader::loadedChunks) {

		FIntVector deltaRange = loadedChunk.Key - cPlayerChunkLoc;

		//Check Terrainmesh < checkedRadius for mesh connection
		if (deltaRange.X < checkedRadiusXY && deltaRange.Y < checkedRadiusXY && deltaRange.Z < checkedRadiusZ) {
			loadedChunk.Value->GenerateTerrainMesh();
		}
		
	}


}

void AWorldGenerator::OnCheckChunk(FIntVector chunkLoc){

	checkedChunkLocs.Add(chunkLoc);

	if (!FWorldLoader::loadedChunks.Contains(chunkLoc)) {
		LoadChunk(chunkLoc);
	}



	if (bDrawDebug) {
		FVector startLoc = ChunkToWorldLocation(chunkLoc);
		FVector endLoc = startLoc + FVector(0, 0, 20000);
		DrawDebugLine(GetWorld(), startLoc, endLoc, FColor::Red, true, 1000, 0, 100);
	}

}



void AWorldGenerator::LoadChunk(FIntVector chunkLoc){

	AChunk* newChunk = AChunk::Construct(this, chunkLoc);
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

		//Continue loop if Z under deathZone, chunks not getting generated under deathZone
		if (center.Z + dz < FWorldParams::deathZone) {
			continue;
		}

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

void AWorldGenerator::PlaceBlock(FIntVector blockLocation, const FResource* resource){

	//TODO maybe Refactor
	TArray<FIntVector> chunkBlockLocs;
	TArray<FIntVector> chunkLocs = BlockToChunkBlockLocation(blockLocation, chunkBlockLocs);

	FWorldLoader::loadedChunks[chunkLocs[0]]->SetBlock(chunkBlockLocs[0], resource);

	for (size_t i = 1; i < chunkLocs.Num(); i++) {
		FWorldLoader::loadedChunks[chunkLocs[i]]->UpdateTerrainMesh(chunkBlockLocs[i]);

	}

}

void AWorldGenerator::HitBlock(FIntVector blockLocation, float damageAmount, AActor* causer){
	FIntVector chunkBlockLoc;
	FIntVector chunkLoc = BlockToChunkBlockLocation(blockLocation, chunkBlockLoc);

	FWorldLoader::loadedChunks[chunkLoc]->HitBlock(chunkBlockLoc, damageAmount, causer);
}

void AWorldGenerator::RemoveBlock(FIntVector blockLocation){

	PlaceBlock(blockLocation, FResource::FromId(this, 0));
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
	return Noise(loc.X, loc.Y, 1);
}

float AWorldGenerator::RainNoise(const FVector2D & loc) const
{
	return Noise(loc.X + 1000, loc.Y + 1000, 1);
}

const TArray<FNoiseParams> AWorldGenerator::GetNoiseParams() const{
	return noiseParams;
}

const FGenerationParams AWorldGenerator::GetGenerationParams() const{
	return generationParams;
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


