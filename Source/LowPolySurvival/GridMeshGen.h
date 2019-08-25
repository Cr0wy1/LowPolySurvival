// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameStructs.h"
#include "GridMeshGen.generated.h"

/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UGridMeshGen : public UObject
{
	GENERATED_BODY()

protected:

	FBlockGrid* blockGrid = nullptr;

public:

	void Init(FBlockGrid*_blockGrid);
	void Create();
	void Update();
};
