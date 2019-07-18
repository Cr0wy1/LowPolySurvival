// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkColumn.generated.h"


class AWorldGenerator;



UCLASS()
class LOWPOLYSURVIVAL_API AChunkColumn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkColumn();

	void Init(AWorldGenerator* _worldGenerator);

protected:

	FIntVector chunkColLoc;
	TArray<TArray<float>> terrainNoiseMap;

	AWorldGenerator* worldGenerator = nullptr;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void InitTerrainNoiseMap();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Create(FIntVector _chunkColLoc);

	const TArray<TArray<float>>* GetTerrainNoiseMap() const;
};
