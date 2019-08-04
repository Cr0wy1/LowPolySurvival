// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveRegion.h"
#include "MyGameInstance.h"

void USaveRegion::SetGrid(const TArray<TArray<TArray<FBlock>>> blockGrid){

	if (blockGrid.Num() != FWorldParams::chunkSize) return;

	FIntVector dim = FIntVector(blockGrid.Num(), blockGrid[0].Num(), blockGrid[0][0].Num());

	grid.yDims.Init(FYDim(), dim.X);
	for (size_t x = 0; x < dim.X; x++){
		grid[x].zDims.Init(FZDim(), dim.Y);
		for (size_t y = 0; y < dim.Y; y++) {
			grid[x][y].Init(FBlock(), dim.Z);
			for (size_t z = 0; z < dim.Z; z++) {
				grid[x][y][z] = blockGrid[x][y][z];
			} 
		}
	}

}
