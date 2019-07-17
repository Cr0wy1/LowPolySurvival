// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

#define PM_GRASS SurfaceType1
#define PM_STONE SurfaceType2
#define PM_WOOD SurfaceType3
#define PM_ICE SurfaceType4

class UDataTable;
class AWorldGenerator;

USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FWorldParams {

	GENERATED_BODY()

	static const float blockSize;
	static const uint8 chunkSize = 16;
	static const float deathZone;
	static const float buildHeight;
};


/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Datatable")
	UDataTable * itemDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Datatable")
	UDataTable * dropsDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Datatable")
	UDataTable * craftingDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Datatable")
	UDataTable * islandDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Datatable")
	UDataTable * worldGenDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGenerator")
	TSubclassOf<AWorldGenerator> worldGenerator_BP;

	AWorldGenerator* worldGenerator = nullptr;

public:
	

	virtual void Init() override;
	void InitDataTables();

	void CreateWorld(FName worldName);

	UDataTable* GetItemTable() const;
	UDataTable* GetDropsTable() const;
	UDataTable* GetCraftingTable() const;
	UDataTable* GetIslandTable() const;
	UDataTable* GetWorldGenTable() const;

	AWorldGenerator* GetWorldGenerator() const;
	
};

FVector WorldToBlockLocation(const FVector &worldLocation);
FVector2D WorldToChunkLocation(const FVector &worldLocation);

FVector BlockToWorldLocation(const FVector &blockLocation);
FVector ChunkToWorldLocation(const FIntVector &chunkLocation);

FVector ChunkToBlockLocation(const FIntVector &chunkLocation);
TArray<FIntVector> BlockToChunkBlockLocation(const FIntVector &blockLocation, TArray<FIntVector> &OUT_chunkBlockLocation);