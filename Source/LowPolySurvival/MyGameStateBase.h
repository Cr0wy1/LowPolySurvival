// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameStateBase.generated.h"


USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FWorldData{
	GENERATED_BODY()

	FName worldName = "None";
	uint8 difficulty = 0;
	FVector spawn = FVector(0,0,0);
	int32 seed = 0;
	int32 worldTime = 0;
	int32 dayTime = 0;
	int32 currentRainTime = 0;
	int32 nextRainTime = 0;
};
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
protected:
	FWorldData worldData;

public:

	const FWorldData GetWorldData() const;

};
