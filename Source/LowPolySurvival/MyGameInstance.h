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
class UWidgetAsset;



USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FWorldParams {

	GENERATED_BODY()

	static const float blockSize;
	static const int32 chunkSize = 16;
	static const int32 terrainBlockLevel = 100;
	static const int32 terrainNoiseHeight = 20;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Datatable")
	UDataTable * resourceDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Datatable")
	UDataTable * biomeDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGenerator")
	TSubclassOf<AWorldGenerator> worldGenerator_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	UWidgetAsset* widgetAsset_A;
	
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
	UDataTable* GetResourceTable() const;
	UDataTable* GetBiomeTable() const;

	AWorldGenerator* GetWorldGenerator() const;
	UWidgetAsset* GetWidgetAsset() const;
	
};

FIntVector WorldToBlockLocation(const FVector &worldLocation);
FIntVector WorldToChunkLocation(const FVector &worldLocation);

FVector BlockToWorldLocation(const FVector &blockLocation);
FIntVector BlockToChunkLocation(const FIntVector &blockLocation);

FVector ChunkToWorldLocation(const FIntVector &chunkLocation);

FIntVector ChunkToBlockLocation(const FIntVector &chunkLocation);

FIntVector BlockToChunkBlockLocation(const FIntVector &blockLocation, FIntVector &OUT_chunkBlockLocation);
TArray<FIntVector> BlockToChunkBlockLocation(const FIntVector &blockLocation, TArray<FIntVector> &OUT_chunkBlockLocation);