// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryWidget.h"
#include "QuickSlotsWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UQuickSlotsWidget : public UInventoryWidget
{
	GENERATED_BODY()

protected:

	int32 currentSlotIndex = 0;
	int32 nextSlotIndex = 0;

	
public:

	
	FItemStack* SelectSlot(int32 slotIndex);
	FItemStack* SelectNextSlot();
	FItemStack* SelectPreviousSlot();

	int32 GetCurrentSelectedSlot() const;
	
};
