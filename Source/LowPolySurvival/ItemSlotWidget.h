// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "Item.h"
#include "ItemSlotWidget.generated.h"


class UButton;
class UImage;
class UItemStackWidget;
class UInventoryWidget;
class UInventoryManagerWidget;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UItemSlotWidget : public UHUDWidget
{
	GENERATED_BODY()
	

protected:
	bool bIsEmpty = true;

	UInventoryWidget* inventoryWidget;
	UInventoryManagerWidget* inventoryManager;

	FItemStack* mouseStack;
	FItemStack* slotStack;


	UPROPERTY(meta = (BindWidget))
	UButton* rootButton;

	UPROPERTY(meta = (BindWidget))
	UItemStackWidget* itemStackWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	EItemType limitedItemType = EItemType::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	UTexture2D* slotImageOverride;

	void OnLeftClick(const FPointerEvent & MouseEvent);
	void OnRightClick(const FPointerEvent & MouseEvent);
	
public:



	virtual bool  Initialize() override;

	UFUNCTION()
	FEventReply OnPreviewMouseButtonDown(FGeometry MyGeometry, const FPointerEvent & MouseEvent);

	UFUNCTION()
	void OnHovered();

	UFUNCTION()
	void OnUnhovered();


	void Init(UInventoryWidget * _inventoryWidget);
	void BindToStack(FItemStack* itemStack);

	bool IsEmpty() const;
	void RefreshSlot();

	FItemStack* GetItemStack() const;

	
};
