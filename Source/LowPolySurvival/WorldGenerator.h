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
class UMyGameInstance;


USTRUCT()
struct LOWPOLYSURVIVAL_API FWorldLoader {
	GENERATED_BODY()

	static TMap<FIntVector, UChunkColumn*> loadedChunkColumns;
	static TMap<FIntVector, AChunk*> loadedChunks;

};



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
	

	UMyGameInstance* gameInstance = nullptr;
	APlayercharController * playerController = nullptr;
	 
	FIntVector cPlayerChunkLoc = FIntVector(0.5f, 0.5f, 0.5f);
	 
	//TMap<FIntVector, AChunk*> loadedChunks;
	TArray<FIntVector> checkedChunkLocs;

	//TMap<FIntVector, UChunkColumn*> loadedChunkColumns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int32 checkedRadiusZ = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int32 checkedRadiusXY = 2;

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

	void OnEnterChunk(); 
	void OnCheckChunk(FIntVector chunkLoc);

	void LoadChunk(FIntVector chunkLoc);
	

public:	
	// Called every frame  
	virtual void Tick(float DeltaTime) override; 

	void CheckChunks(FIntVector center) ;

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

	FBiomeData* GetBiome(float heatNoise, float rainNoise) const;
	FBiomeData* GetBiome(const FBlockLoc &blockLoc) const;
    AChunk* GetChunk(const FChunkLoc &chunkLoc) const;
	AChunk* GetChunkFromBlock(const FBlockLoc &blockLoc) const;

	//Check if chunk exist, return nullptr if doesn't
	AChunk* GetChunkSafe(const FIntVector &chunkLoc) const;
	int32 GetLoadedChunksNum() const;

	FString GetWorldName() const;

	bool IsChunkLoaded(const FIntVector &chunkLoc) const;

};
