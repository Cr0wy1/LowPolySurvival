// Fill out your copyright notice in the Description page of Project Settings.


#include "GridVoxelMeshGen.h"
#include "MyGameInstance.h"
#include "Chunk.h"

static int32 TASKCOUNTER = 0;

void UGridVoxelMeshGen::DoTaskWork(bool bUpdateOnly, const FBlockGrid &blockGrid) {

	//UE_LOG(LogTemp, Warning, TEXT("VoxelMesh: BeginTask"));
	meshGenData.Reset();

	const FBlockGrid* grid = params.chunk->GetGridData();

	for (size_t x = 0; x < gridSize.X; x++) {
		for (size_t y = 0; y < gridSize.Y; y++) {
			for (size_t z = 0; z < gridSize.Z; z++) {

				if ((*grid)[x][y][z].data.bIsSolid) {
					BuildCube(FIntVector(x, y, z), grid);
				}

			}
		}
	}

	//UMeshGenerationLibrary::CalculateNormalsAndTangents(meshGenData, 0.0f);

	TASKCOUNTER++;
	//UE_LOG(LogTemp, Warning, TEXT("VoxelMesh: EndTask %i"), TASKCOUNTER);
	//UE_LOG(LogTemp, Warning, TEXT("meshData: %s"), *meshGenData.ToString());

	Super::DoTaskWork(bUpdateOnly, blockGrid);
}

void UGridVoxelMeshGen::BuildCube(const FIntVector &blockLoc, const FBlockGrid* grid){

	TArray<AChunk*> nearbyChunks = params.chunk->GetNearbyChunks();

	const FBlock* nearbyBlock;

	//FRONT
	if (blockLoc.X == 15 && nearbyChunks[0]) {
		nearbyBlock = nearbyChunks[0]->GetBlock(FIntVector(0, blockLoc.Y, blockLoc.Z));
	}else {
		nearbyBlock = grid->GetBlockSafe(blockLoc + FGridDir::FRONT);
	}

	if (nearbyBlock && !nearbyBlock->data.bIsSolid) {
		BuildCubeSite(0, blockLoc, grid);
	}

	//RIGHT
	if (blockLoc.Y == 15 && nearbyChunks[1]) {
		nearbyBlock = nearbyChunks[1]->GetBlock(FIntVector(blockLoc.X, 0, blockLoc.Z));
	}
	else {
		nearbyBlock = grid->GetBlockSafe(blockLoc + FGridDir::RIGHT);
	}
	
	if (nearbyBlock && !nearbyBlock->data.bIsSolid) {
		BuildCubeSite(1, blockLoc, grid);
	}

	//UP
	if (blockLoc.Z == 15 && nearbyChunks[2]) {
		nearbyBlock = nearbyChunks[2]->GetBlock(FIntVector(blockLoc.X, blockLoc.Y, 0));
	}
	else {
		nearbyBlock = grid->GetBlockSafe(blockLoc + FGridDir::UP);
	}
	if (nearbyBlock && !nearbyBlock->data.bIsSolid) {
		BuildCubeSite(2, blockLoc, grid);
	}

	//BACK
	if (blockLoc.X == 0 && nearbyChunks[3]) {
		nearbyBlock = nearbyChunks[3]->GetBlock(FIntVector(15, blockLoc.Y, blockLoc.Z));
	}
	else {
		nearbyBlock = grid->GetBlockSafe(blockLoc + FGridDir::BACK);
	}
	if (nearbyBlock && !nearbyBlock->data.bIsSolid) {
		BuildCubeSite(3, blockLoc, grid);
	}


	//LEFT
	if (blockLoc.Y == 0 && nearbyChunks[4]) {
		nearbyBlock = nearbyChunks[4]->GetBlock(FIntVector(blockLoc.X, 15, blockLoc.Z));
	}
	else {
		nearbyBlock = grid->GetBlockSafe(blockLoc + FGridDir::LEFT);
	}
	if (nearbyBlock && !nearbyBlock->data.bIsSolid) {
		BuildCubeSite(4, blockLoc, grid);
	}

	//LEFT
	if (blockLoc.Z == 0 && nearbyChunks[5]) {
		nearbyBlock = nearbyChunks[5]->GetBlock(FIntVector(blockLoc.X, blockLoc.Y, 15));
	}
	else {
		nearbyBlock = grid->GetBlockSafe(blockLoc + FGridDir::DOWN);
	}
	if (nearbyBlock && !nearbyBlock->data.bIsSolid) {
		BuildCubeSite(5, blockLoc, grid);
	}
	
}

void UGridVoxelMeshGen::BuildCubeSite(int32 siteIndex, const FIntVector &blockLoc, const FBlockGrid* grid){

	int32 vertIndex;
	for (size_t vi = 0; vi < 4; vi++) {

		FVector vertPos = BlockToWorldLocation(vertTable[quadTable[siteIndex][vi]] + FVector(blockLoc) );

		vertIndex = meshGenData.vertexArray.Add(vertPos);
		meshGenData.triangles.Add(vertIndex);
		meshGenData.vertColors.Add((*grid)[blockLoc.X][blockLoc.Y][blockLoc.Z].data.color);
		meshGenData.normals.Add(normalsTable[siteIndex]);
	}

	float uvBorder = 0.5f;// 16.0f / 256.0f;
	meshGenData.UVs.Add(FVector2D(0, 0));
	meshGenData.UVs.Add(FVector2D(0.0f, uvBorder));
	meshGenData.UVs.Add(FVector2D(uvBorder, 0.0f));
	meshGenData.UVs.Add(FVector2D(uvBorder, uvBorder));

	meshGenData.triangles.Add(vertIndex - 1);
	meshGenData.triangles.Add(vertIndex - 2);

}
