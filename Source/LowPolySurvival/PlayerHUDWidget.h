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
class UTextBlock;
class UPanelWidget;
struct FAttributes;
struct FBuildingInfo;
struct FBlock;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UPlayerHUDWidget : public UHUDWidget
{
	GENERATED_BODY()
	

public:

	FAttributes * playerAttributes;
	UInventoryComponent* equipInventoryComp = nullptr;

	UPROPERTY(meta = (BindWidget))
	UQuickSlotsWidget* playerQuickInv;
	
	UPROPERTY(meta = (BindWidget))
	UHeartWidget* heartWidget;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* indicatorBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* nameIndicatorText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* damageIndicatorProgress;


public:

	void Init(FAttributes * attributes, UInventoryComponent* _equipInventoryComp);
	
	void BindQuickSlot(UInventoryComponent *quickInvComp, UInventoryManagerWidget* _inventoryManager);

	FItemStack* OnScrollDown();
	FItemStack* OnScrollUp();

	void UpdateHealth();
	void UpdateArmor();

	void UpdateTargetIndicator(const FBuildingInfo &targetInfo);
	void UpdateTargetIndicator(const FBlock* targetInfo);
	void ShowTargetIndicator();
	void HideTargetIndicator();

	UFUNCTION()
	void OnEquipCompUpdate(const TArray<int32> &updatedSlots);

	UFUNCTION(BlueprintCallable)
	int32 GetPlayerHealth() const;

	int32 GetCurrentSelectedQuickSlot() const;
};
