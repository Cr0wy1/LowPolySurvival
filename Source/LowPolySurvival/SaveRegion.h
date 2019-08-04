// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameStructs.h"
#include "SaveRegion.generated.h"

USTRUCT()
struct LOWPOLYSURVIVAL_API FSaveBlock {

	GENERATED_BODY()

	UPROPERTY()
	int32 itemId = 0;

	UPROPERTY()
	int32 resourceId = 0;

};


USTRUCT()
struct LOWPOLYSURVIVAL_API FZDim {

	GENERATED_BODY()

	UPROPERTY()
	TArray<FSaveBlock> blocks;


	FSaveBlock& operator[](int32 index) {
		return blocks[index];
	}

	const FSaveBlock& operator[](int32 index)const {
		return blocks[index];
	}
};


USTRUCT()
struct LOWPOLYSURVIVAL_API FYDim {

	GENERATED_BODY()

	UPROPERTY()
	TArray<FZDim> zDims;


	TArray<FSaveBlock>& operator[](int32 index) {
		return zDims[index].blocks;
	}

	const TArray<FSaveBlock>& operator[](int32 index)const {
		return zDims[index].blocks;
	}

	

};

USTRUCT()
struct LOWPOLYSURVIVAL_API FXDim {

	GENERATED_BODY()

	UPROPERTY()
	TArray<FYDim> yDims;


	FYDim& operator[](int32 index) {
		return yDims[index];
	}

	const FYDim& operator[](int32 index)const {
		return yDims[index];
	}

};
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API USaveRegion : public USaveGame
{

	GENERATED_BODY()


public:

	UPROPERTY()
	FXDim grid;

	UPROPERTY()
	FName testName = "test";


	void SetGrid(const TArray<TArray<TArray<FBlock>>> blockGrid);
	void GetGrid(AActor* contextActor, TArray<TArray<TArray<FBlock>>> &OUT_blockGrid);
}; 
