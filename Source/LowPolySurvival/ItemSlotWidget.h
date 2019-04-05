// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "ItemSlotWidget.generated.h"


class UButton;
class UImage;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UItemSlotWidget : public UHUDWidget
{
	GENERATED_BODY()
	
	
public:

	virtual bool  Initialize() override;

	UPROPERTY(meta = (BindWidget))
	UButton* slotButton;

	UPROPERTY(meta = (BindWidget))
	UImage* slotImage;
	
};
