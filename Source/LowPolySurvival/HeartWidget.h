// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "HeartWidget.generated.h"


class UProgressBar;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UHeartWidget : public UHUDWidget
{
	GENERATED_BODY()
	
	
protected:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* heartProgress;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* heartText;

	UPROPERTY(meta = (BindWidget))
	UImage* armorImage;
	
public:

	void SetHeartValues(float percentProgress, int32 amountText);
	void SetArmorPercent(float percent);
};
