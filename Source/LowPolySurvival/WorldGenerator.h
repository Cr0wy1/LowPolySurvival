// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "SimplexNoise.h"
#include "GameStructs.h"
#include "WorldGenerator.generated.h"


class APlayercharController;
class AChunk;
class UChunkColumn;
class UMaterialInterface;


UCLASS()
class LOWPOLYSURVIVAL_API AWorldGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldGenerator();

protected:

	bool bDrawDebug = false;

	FString worldName = "world";
	
	int32 checkedRadiusZ = 1;
	int32 checkedRadiusXY = 1; 
	 
	APlayercharController * playerController = nullptr;
	 
	FIntVector cPlayerChunkLoc = FIntVector(0.5f, 0.5f, 0.5f);
	 
	TMap<FIntVector, AChunk*> loadedChunks;
	TArray<FIntVector> checkedChunkLocs;

	TMap<FIntVector, UChunkColumn*> loadedChunkColumns;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	FNoiseParams noiseParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	FNoiseParams caveNoiseParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	FNoiseParams oreNoiseParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	FGenerationParams generationParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	UMaterialInterface* terrainMaterial;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnEnterChunk(); 
	void OnCheckChunk(FIntVector chunkLoc);

	void LoadChunk(FIntVector chunkLoc);
	

public:	
	// Called every frame  
	virtual void Tick(float DeltaTime) override; 

	void CheckChunks(FIntVector center) ;

	void PlaceBlock(FIntVector blockLocation, const FBlockData &blockData);
	void HitBlock(FIntVector blockLocation, float damageAmount, AActor* causer);
	void RemoveBlock(FIntVector blockLocation);

	float TerrainNoise(const FVector2D &loc) const;
	float CaveNoise(const FVector &loc) const;
	float OreNoise(const FVector &loc) const;
	 
	const FNoiseParams GetNoiseParams() const;
	const FGenerationParams GetGenerationParams() const;
	AChunk* GetChunk(const FIntVector &chunkLoc) const;

	//Check if chunk exist, return nullptr if doesn't
	AChunk* GetChunkSafe(const FIntVector &chunkLoc) const;
	int32 GetLoadedChunksNum() const;

	FString GetWorldName() const;

	bool IsChunkLoaded(const FIntVector &chunkLoc) const;

};
