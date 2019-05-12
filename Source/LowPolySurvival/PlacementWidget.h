// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlacementWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UPlacementWidget : public UUserWidget
{
	GENERATED_BODY()
	
	

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placement")
	int32 hoveredIndex = -1;
	
};
