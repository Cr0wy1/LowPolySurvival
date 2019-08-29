// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveRegion.h"
#include "MyGameInstance.h"

void USaveRegion::SetGrid(const FBlockGrid &blockGrid){

	if (blockGrid.dims.X != FWorldParams::chunkSize) return;

	FIntVector dim = blockGrid.dims;

	grid.yDims.Init(FYDim(), dim.X);
	for (size_t x = 0; x < dim.X; x++){
		grid[x].zDims.Init(FZDim(), dim.Y);
		for (size_t y = 0; y < dim.Y; y++) {
			grid[x][y].Init(FSaveBlock(), dim.Z);
			for (size_t z = 0; z < dim.Z; z++) {
				//grid[x][y][z].itemId = blockGrid[x][y][z].data.blockId;
				if (blockGrid[x][y][z].resource) {
					grid[x][y][z].resourceId = blockGrid[x][y][z].resource->id;
				}
				
			} 
		}
	}

	testName = "WORLD";
}

void USaveRegion::GetGrid(AActor* contextActor, FBlockGrid& OUT_blockGrid){
	
	FIntVector dim = FIntVector(grid.yDims.Num(), grid[0].zDims.Num(), grid[0][0].Num());

	OUT_blockGrid.Init(dim.X, dim.Y, dim.Z);

	for (size_t x = 0; x < dim.X; x++) {
		for (size_t y = 0; y < dim.Y; y++) {
			for (size_t z = 0; z < dim.Z; z++) {
				
				OUT_blockGrid[x][y][z].SetResource(FResource::FromId(contextActor, grid[x][y][z].resourceId));
				
			}
		}
	}
}
