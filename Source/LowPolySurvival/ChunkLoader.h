// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameStructs.h"
#include "ChunkLoader.generated.h"



class AWorldGenerator;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UChunkLoader : public UObject
{
	GENERATED_BODY()
		 
public:

	static UChunkLoader* Construct(AWorldGenerator* _worldGenerator, int32 radiusXY, int32 radiusZ);

	void Init(AWorldGenerator* _worldGenerator, int32 radiusXY, int32 radiusZ);

	void UpdateLocation(FChunkLoc newCenterChunkLoc);

protected:

	FChunkLoc centerChunkLoc;

	AWorldGenerator * worldGenerator;

	TArray<FIntVector> checkedChunkLocs;

	int32 checkedRadiusZ = 2;
	int32 checkedRadiusXY = 2;

	void OnCheckChunk(FIntVector chunkLoc);
	void LoadChunk(FIntVector chunkLoc);
	void CheckChunks(FIntVector center);
};
