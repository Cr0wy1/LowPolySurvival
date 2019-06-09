// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "CraftingWidget.generated.h"


class UItemSlotGridWidget;
struct FCraftingInfo;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UCraftingWidget : public UHUDWidget
{
	GENERATED_BODY()

protected:
	TArray<FCraftingInfo*> craftingInfoArr;

	UPROPERTY(meta = (BindWidget))
	UItemSlotGridWidget* slotGrid;

public:

	void Init();
};