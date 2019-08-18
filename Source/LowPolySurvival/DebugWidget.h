// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "DebugWidget.generated.h"


class UTextBlock;
/**
 * 
 */

USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FDebugInfo {

	GENERATED_BODY()

	FVector playerLocation;
	FVector playerVelocity;
	FRotator playerLookDirection;
	FName biomeName;

	int32 loadedChunks;
};




UCLASS()
class LOWPOLYSURVIVAL_API UDebugWidget : public UHUDWidget
{
	GENERATED_BODY()
	

public:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* playerLocText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* playerBlockLocText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* playerChunkLocText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* loadedChunksText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* loadedBlocksText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* playerLookDirText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* playerVelText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* biomeText;

	void Update(const FDebugInfo &debugInfo);
	

	FString GetCompassFromRotation(const float yaw) const;
};
