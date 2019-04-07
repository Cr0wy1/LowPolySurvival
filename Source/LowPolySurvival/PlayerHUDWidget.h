// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "PlayerHUDWidget.generated.h"



class UInventoryWidget;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UPlayerHUDWidget : public UHUDWidget
{
	GENERATED_BODY()
	
	
public:

	UPROPERTY(meta = (BindWidget))
	UInventoryWidget* inventory;

	void ToggleInventory();
	
};
