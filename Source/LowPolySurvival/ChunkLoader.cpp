// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkLoader.h"
#include "Engine/World.h"
#include "WorldGenerator.h"
#include "Chunk.h"
#include "MyGameInstance.h"


UChunkLoaderComponent::UChunkLoaderComponent(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer) {
	
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UChunkLoaderComponent::BeginPlay(){
	Super::BeginPlay();

	owner = GetOwner();

	UMyGameInstance* gameInstance = owner->GetGameInstance<UMyGameInstance>();
	worldGenerator = gameInstance->GetWorldGenerator();

	if (bIsDynamic) {
		SetComponentTickEnabled(true);
	}
	else {
		SetComponentTickEnabled(false);
		FIntVector chunkLoc = WorldToChunkLocation(owner->GetActorLocation());
		UpdateLocation(chunkLoc);
	}
}


void UChunkLoaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (owner) {
		FIntVector chunkLoc = WorldToChunkLocation(owner->GetActorLocation());
		UpdateLocation(chunkLoc);
	}

}


void UChunkLoaderComponent::UpdateLocation(FChunkLoc newCenterChunkLoc){

	if (newCenterChunkLoc == cChunkLoc) return;
	cChunkLoc = newCenterChunkLoc;

	CheckChunks(cChunkLoc);


	TArray<FIntVector> chunksToUnload;

	//collect unloaded chunks
	for (auto loadedChunk : AWorldGenerator::loader.loadedChunks) {
		if (!checkedChunkLocs.Contains(loadedChunk.Key)) {
			chunksToUnload.Add(loadedChunk.Key);
		}
	}

	//Unload unloaded chunks
	for (const FIntVector loc : chunksToUnload) {

		AWorldGenerator::loader.loadedChunks[loc]->Unload();
		AWorldGenerator::loader.loadedChunks.Remove(loc);
		//UE_LOG(LogTemp, Warning, TEXT("removed"));
	}


	checkedChunkLocs.Empty();

	//Generate Terrain Mesh for loaded Chunks
	for (auto loadedChunk : AWorldGenerator::loader.loadedChunks) {

		FIntVector deltaRange = loadedChunk.Key - cChunkLoc;

		//Check Terrainmesh < checkedRadius for mesh connection
		if (deltaRange.X < checkedRadiusXY && deltaRange.Y < checkedRadiusXY && deltaRange.Z < checkedRadiusZ) {
			loadedChunk.Value->GenerateTerrainMesh();
		}

	}

}





void UChunkLoaderComponent::OnCheckChunk(FIntVector chunkLoc) {

	int32 chunkSize = 10;
	//UE_LOG(LogTemp, Warning, TEXT("loadedChunkCols: %i"), FWorldLoader::loadedChunkColumns.Num());

	if (chunkLoc.X < chunkSize && chunkLoc.Y < chunkSize && chunkLoc.X > chunkSize*-1 && chunkLoc.Y > chunkSize*-1) {

		checkedChunkLocs.Add(chunkLoc);

		if (!AWorldGenerator::loader.loadedChunks.Contains(chunkLoc)) {
			LoadChunk(chunkLoc);
		}

	}
}



void UChunkLoaderComponent::LoadChunk(FIntVector chunkLoc) {

	AChunk* newChunk = AChunk::Construct(worldGenerator, chunkLoc);
}

void UChunkLoaderComponent::CheckChunks(FIntVector center) {

	int32 radiusXY = checkedRadiusXY * 2 + 1;
	int32 radiusZ = checkedRadiusZ * 2 + 1;

	int32 dz = 0;
	int32 zDir = -1;
	for (int32 z = 0; z < radiusZ; ++z) {

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