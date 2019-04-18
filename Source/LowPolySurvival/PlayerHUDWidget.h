// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "InventoryComponent.h"
#include "PlayerHUDWidget.generated.h"



class UInventoryWidget;
class UInventoryManagerWidget;
class UQuickSlotsWidget;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UPlayerHUDWidget : public UHUDWidget
{
	GENERATED_BODY()
	

protected:

	UPROPERTY(meta = (BindWidget))
	UQuickSlotsWidget* playerQuickInv;

	
public:
	
	void BindQuickSlot(UInventoryComponent *quickInvComp, UInventoryManagerWidget* _inventoryManager);

	FItemStack* OnScrollDown();
	FItemStack* OnScrollUp();
};
