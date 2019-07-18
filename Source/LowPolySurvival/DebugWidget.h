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

	void Update(const FDebugInfo &debugInfo);
	
};
