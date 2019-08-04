// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameStructs.h"
#include "SaveRegion.generated.h"




USTRUCT()
struct LOWPOLYSURVIVAL_API FZDim {

	GENERATED_BODY()

	TArray<FBlock> blocks;


	FBlock& operator[](int32 index) {
		return blocks[index];
	}

	const FBlock& operator[](int32 index)const {
		return blocks[index];
	}
};


USTRUCT()
struct LOWPOLYSURVIVAL_API FYDim {

	GENERATED_BODY()

	TArray<FZDim> zDims;


	TArray<FBlock>& operator[](int32 index) {
		return zDims[index].blocks;
	}

	const TArray<FBlock>& operator[](int32 index)const {
		return zDims[index].blocks;
	}

	

};

USTRUCT()
struct LOWPOLYSURVIVAL_API FXDim {

	GENERATED_BODY()

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

	void SetGrid(const TArray<TArray<TArray<FBlock>>> blockGrid);
};
