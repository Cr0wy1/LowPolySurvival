// Fill out your copyright notice in the Description page of Project Settings.


#include "GridVoxelMeshGen.h"
#include "MyGameInstance.h"

void UGridVoxelMeshGen::DoTaskWork(bool bUpdateOnly) {

	for (size_t x = 0; x < gridSize.X; x++) {
		for (size_t y = 0; y < gridSize.Y; y++) {
			for (size_t z = 0; z < gridSize.Z; z++) {

				BuildCube(FVector(x, y, z));

			}
		}
	}

}

void UGridVoxelMeshGen::BuildCube(const FVector &blockLoc){

	int32 quadNum = quadTable.Num();
	for (size_t i = 0; i < quadNum; i++) {
		for (size_t vi = 0; vi < 6; vi++) {

			FVector vertPos = BlockToWorldLocation(blockLoc + vertTable[quadTable[i][vi]] * 0.5f);

			int32 vertIndex = meshGenData.vertexArray.Add(vertPos);
			meshGenData.triangles.Add(vertIndex);
			//meshGenData.vertColors.Add(block)
		}
	}

	
}
