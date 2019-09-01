// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkLoader.h"
#include "Engine/World.h"
#include "WorldGenerator.h"
#include "Chunk.h"
#include "MyGameInstance.h"



UChunkLoader* UChunkLoader::Construct(AWorldGenerator* _worldGenerator, int32 radiusXY, int32 radiusZ) {
	UChunkLoader* chunkLoader = NewObject<UChunkLoader>(_worldGenerator);
	chunkLoader->Init(_worldGenerator, radiusXY, radiusZ);
	return chunkLoader; 
}



void UChunkLoader::Init(AWorldGenerator* _worldGenerator, int32 radiusXY, int32 radiusZ) {
	worldGenerator = _worldGenerator;
	checkedRadiusXY = radiusXY;
	checkedRadiusZ = radiusZ;
}


void UChunkLoader::UpdateLocation(FChunkLoc newCenterChunkLoc){

	if (newCenterChunkLoc == centerChunkLoc) return;
	centerChunkLoc = newCenterChunkLoc;

	CheckChunks(centerChunkLoc);


	TArray<FIntVector> chunksToUnload;

	//collect unloaded chunks
	for (auto loadedChunk : FWorldLoader::loadedChunks) {
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

		FIntVector deltaRange = loadedChunk.Key - centerChunkLoc;

		//Check Terrainmesh < checkedRadius for mesh connection
		if (deltaRange.X < checkedRadiusXY && deltaRange.Y < checkedRadiusXY && deltaRange.Z < checkedRadiusZ) {
			loadedChunk.Value->GenerateTerrainMesh();
		}

	}

}

void UChunkLoader::OnCheckChunk(FIntVector chunkLoc) {

	checkedChunkLocs.Add(chunkLoc);

	if (!FWorldLoader::loadedChunks.Contains(chunkLoc)) {
		LoadChunk(chunkLoc);
	}

}



void UChunkLoader::LoadChunk(FIntVector chunkLoc) {

	AChunk* newChunk = AChunk::Construct(worldGenerator, chunkLoc);
}

void UChunkLoader::CheckChunks(FIntVector center) {

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