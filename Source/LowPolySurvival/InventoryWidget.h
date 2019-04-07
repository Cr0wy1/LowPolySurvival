// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "InventoryWidget.generated.h"


class UItem;
class UItemSlotWidget;
struct FItemInfo;
struct FItemStack;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UInventoryWidget : public UHUDWidget
{
	GENERATED_BODY()


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot Widget")
	TSubclassOf<UItemSlotWidget> itemSlotWidget_W;
	
	TArray<UItemSlotWidget*> slots;

	UPROPERTY(EditAnywhere)
	int32 rows = 4;

	UPROPERTY(EditAnywhere)
	int32 cols = 4;

public:

	virtual bool  Initialize() override;

	void AddStack(FItemStack* itemstack);
	bool AddToExistingStacks(FItemStack* itemstack);
	void AddToEmptySlots(FItemStack* itemstack);
	
};
