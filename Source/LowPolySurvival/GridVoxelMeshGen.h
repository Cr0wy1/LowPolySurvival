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

	

	virtual void DoTaskWork(bool bUpdateOnly) override;

	void BuildCube(const FVector &blockLoc);

	TArray<FVector> vertTable = {
		FVector(1,1,1),
		FVector(1,-1,1),
		FVector(-1,1,1),
		FVector(-1,-1,1),
		FVector(1,1,-1),
		FVector(1,-1,-1),
		FVector(-1,1,-1),
		FVector(-1,-1,-1),
	};

	TArray<TArray<uint8>> quadTable = {
		{0, 1, 2, 1, 2, 3},
		{0, 1, 4, 1, 4, 5},
		
	};
};
