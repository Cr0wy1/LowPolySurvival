// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkColumn.generated.h"


class AWorldGenerator;



UCLASS(BlueprintType, Blueprintable)
class LOWPOLYSURVIVAL_API UChunkColumn : public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UChunkColumn();

	virtual void FinishDestroy() override;

	void Init(AWorldGenerator* _worldGenerator);

protected:

	FIntVector chunkColLoc;
	TArray<TArray<float>> hillsNoiseMap;
	TArray<TArray<float>> terrainNoiseMap;
	TArray<TArray<float>> heatNoiseMap;
	TArray<TArray<float>> rainNoiseMap;

	AWorldGenerator* worldGenerator = nullptr;
	

	void Create();

	void InitTerrainNoiseMaps();

public:	

	static UChunkColumn* Construct(AWorldGenerator* _worldGenerator, FIntVector _chunkColLoc);

	void Load(FIntVector _chunkColLoc);
	void Unload();

	const TArray<TArray<float>>* GetTerrainNoiseMap() { return &terrainNoiseMap; }
	const TArray<TArray<float>>* GetHillsNoiseMap() { return &hillsNoiseMap;  } 
	const TArray<TArray<float>>* GetHeatNoiseMap() { return &heatNoiseMap; }
	const TArray<TArray<float>>* GetRainNoiseMap() { return &rainNoiseMap; }
};
