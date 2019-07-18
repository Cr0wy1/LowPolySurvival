// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "SimplexNoise.h"
#include "WorldGenerator.generated.h"


class APlayercharController;
class AChunk;
class AChunkColumn;
class UMaterialInterface;


UENUM(BlueprintType)
enum class EBiome : uint8 {
	GRASS,
	FOREST,
	SAND,
	HILLS,

};


USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FWorldGenInfo : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 id = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Plant_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBiome biome = EBiome::GRASS;
};





USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FGenerationParams {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float surfaceLevel = 0.2f;

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

	

	int32 checkedRadius = 2;
	 
	APlayercharController * playerController = nullptr;
	 
	FIntVector cPlayerChunkLoc = FIntVector(0.5f, 0.5f, 0.5f);
	 
	TArray<FIntVector> createdChunks;
	TMap<FIntVector, AChunk*> loadedChunks;
	TArray<FIntVector> checkedChunkLocs;

	TMap<FIntVector, AChunkColumn*> loadedChunkColumns;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	FNoiseParams noiseParams;

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

	void RemoveBlock(FIntVector blockLocation);

	float TerrainNoise(const FVector2D &loc) const;
	 
	const FNoiseParams GetNoiseParams() const;
	const FGenerationParams GetGenerationParams() const;
	AChunk* GetChunk(const FIntVector &chunkLoc) const;

	//Check if chunk exist, return nullptr if doesn't
	AChunk* GetChunkSafe(const FIntVector &chunkLoc) const;

	bool IsChunkLoaded(const FIntVector &chunkLoc) const;

};
