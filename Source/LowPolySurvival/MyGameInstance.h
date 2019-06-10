// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"


class UDataTable;
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

public:

	virtual void Init() override;
	void InitDataTables();

	UDataTable* GetItemTable() const;
	UDataTable* GetDropsTable() const;
	UDataTable* GetCraftingTable() const;
	UDataTable* GetIslandTable() const;
	
};
