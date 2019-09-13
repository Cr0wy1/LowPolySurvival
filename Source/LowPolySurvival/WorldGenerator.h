// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "SimplexNoise.h"
#include "GameStructs.h"
#include "WorldGenerator.generated.h"



class AChunk;
class UChunkColumn;
class UMaterialInterface;



USTRUCT()
struct LOWPOLYSURVIVAL_API FWorldLoader {
	GENERATED_BODY()


	TMap<FIntVector, UChunkColumn*> loadedChunkColumns;
	TMap<FIntVector, AChunk*> loadedChunks;

	FString ToString() {
		return "Loaded: chunkColsNum:" + FString::FromInt(loadedChunkColumns.Num()) + "chunksNum:" + FString::FromInt(loadedChunks.Num());
	}
};



UCLASS()
class LOWPOLYSURVIVAL_API AWorldGenerator : public AActor
{
	GENERATED_BODY()



	
public:	
	// Sets default values for this actor's properties
	AWorldGenerator();

	static FWorldLoader loader;

protected:


	bool bDrawDebug = false;

	FString worldName = "world";
	int32 seed = 0;
	 
	//TMap<FIntVector, AChunk*> loadedChunks;
	TArray<FIntVector> checkedChunkLocs;

	//TMap<FIntVector, UChunkColumn*> loadedChunkColumns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	FNoiseParams biomeNoiseParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	TArray<FNoiseParams> noiseParams;

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

public:	
	// Called every frame  
	virtual void Tick(float DeltaTime) override; 

	void CleanAllChunks();

	void HitBlock(FIntVector blockLocation, float damageAmount, AActor* causer);
	void RemoveBlock(FIntVector blockLocation);

	float BiomeNoise(const FVector2D &loc) const;
	float TerrainNoise(const FVector2D &loc) const;
	float CaveNoise(const FVector &loc) const;
	float OreNoise(const FVector &loc) const;

	float HeatNoise(const FVector2D &loc) const;
	float RainNoise(const FVector2D &loc) const;
	 
	const TArray<FNoiseParams> GetNoiseParams() const;
	const FGenerationParams GetGenerationParams() const;

	bool SetBlock(const FBlockLoc &blockLoc, const FBlock &block);
	const FBlock* GetBlock(const FBlockLoc &blockLoc);

	FBiomeData* GetBiome(float xNoise, float yNoise) const;
	FBiomeData* GetBiome(const FBlockLoc &blockLoc) const;
    AChunk* GetChunk(const FChunkLoc &chunkLoc) const;
	AChunk* GetChunkFromBlock(const FBlockLoc &blockLoc) const;

	//Check if chunk exist, return nullptr if doesn't
	AChunk* GetChunkSafe(const FIntVector &chunkLoc) const;
	int32 GetLoadedChunksNum() const;

	FString GetWorldName() const;
	int32 GetSeed() const;
	

	bool IsChunkLoaded(const FIntVector &chunkLoc) const;

};
