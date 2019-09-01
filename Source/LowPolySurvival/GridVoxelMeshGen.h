// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridMeshGen.h"
#include "GridVoxelMeshGen.generated.h"

/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UGridVoxelMeshGen : public UGridMeshGen
{
	GENERATED_BODY()
	

protected:

	

	virtual void DoTaskWork(bool bUpdateOnly, const FBlockGrid &blockGrid) override;

	void BuildCube(const FIntVector &blockLoc, const FBlockGrid* grid);
	void BuildCubeSite(int32 siteIndex, const FIntVector &blockLoc, const FBlockGrid* grid);

	TArray<FVector> vertTable = {
		FVector( 0.5f,-0.5f,-0.5f),
		FVector( 0.5f, 0.5f,-0.5f),
		FVector(-0.5f, 0.5f,-0.5f),
		FVector(-0.5f,-0.5f,-0.5f),
		FVector( 0.5f,-0.5f, 0.5f),
		FVector( 0.5f, 0.5f, 0.5f),
		FVector(-0.5f, 0.5f, 0.5f),
		FVector(-0.5f,-0.5f, 0.5f),

	};

	TArray<FVector> normalsTable = {
		FVector(1,0,0),
		FVector(0,1,0),
		FVector(0,0,1),
		FVector(-1,0,0),
		FVector(0,-1,0),
		FVector(0,0,-1),
	};

	TArray<TArray<uint8>> quadTable = {
		{ 4, 5, 0, 1, 0, 5 }, //FRONT
		{ 5, 6, 1, 2, 1, 6 }, //RIGHT 
		{ 7, 6, 4, 5, 4, 6 }, //TOP
		{ 6, 7, 2, 3, 2, 7 }, //BACK
		{ 7, 4, 3, 0, 3, 4 }, //LEFT
		{0, 1, 3, 2, 3, 1}, //BOTTOM

	};
};
