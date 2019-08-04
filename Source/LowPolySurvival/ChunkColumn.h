// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkColumn.generated.h"


class AWorldGenerator;



UCLASS()
class LOWPOLYSURVIVAL_API UChunkColumn : public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UChunkColumn();

	void Init(AWorldGenerator* _worldGenerator);

protected:

	FIntVector chunkColLoc;
	TArray<TArray<float>> terrainNoiseMap;

	AWorldGenerator* worldGenerator = nullptr;
	
	void InitTerrainNoiseMap();

public:	


	void Create(FIntVector _chunkColLoc);

	const TArray<TArray<float>>* GetTerrainNoiseMap() const;
};
