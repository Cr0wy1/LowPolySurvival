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


USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FWorldInfo {

	GENERATED_BODY()

	float blockSize = 100;
	float chunkSize = 10000;
	float deathZone = -10000;
	float buildHeight = 20000;
	

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

public:
	
	const FWorldInfo* worldInfo = new FWorldInfo();

	virtual void Init() override;
	void InitDataTables();

	UDataTable* GetItemTable() const;
	UDataTable* GetDropsTable() const;
	UDataTable* GetCraftingTable() const;
	UDataTable* GetIslandTable() const;
	UDataTable* GetWorldGenTable() const;
	FWorldInfo const *const GetWorldInfo() const;
	
};
