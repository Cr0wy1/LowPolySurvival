// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "InventoryComponent.h"
#include "PlayerHUDWidget.generated.h"



class UInventoryWidget;
class UInventoryManagerWidget;
class UQuickSlotsWidget;
class UProgressBar;
class UHeartWidget;
struct FAttributes;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UPlayerHUDWidget : public UHUDWidget
{
	GENERATED_BODY()
	

public:

	UPROPERTY(meta = (BindWidget))
	UQuickSlotsWidget* playerQuickInv;

	FAttributes * playerAttributes;
	
	UPROPERTY(meta = (BindWidget))
	UHeartWidget* heartWidget;

public:

	void Init(FAttributes * attributes);
	
	void BindQuickSlot(UInventoryComponent *quickInvComp, UInventoryManagerWidget* _inventoryManager);

	FItemStack* OnScrollDown();
	FItemStack* OnScrollUp();

	void UpdateHealth();

	UFUNCTION(BlueprintCallable)
	int32 GetPlayerHealth() const;
};
