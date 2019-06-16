// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlotWidget.h"
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
class LOWPOLYSURVIVAL_API UItemSlotWidget : public USlotWidget
{
	GENERATED_BODY() 
	

protected:
	bool bIsEmpty = true;

	int32 index = -1;

	UInventoryWidget* inventoryWidget;
	UInventoryManagerWidget* inventoryManager;

	FItemStack* mouseStack;
	FItemStack const* slotStack;

	FSlateBrush normalSlateBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	UTexture2D* slotSelectedIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	EItemType limitedItemType = EItemType::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	int32 limitedTypeIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	UTexture2D* slotImageOverride;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	float slotPaddingOverride = 5.0f;

	virtual void OnLeftClick(const FPointerEvent & MouseEvent) override;
	virtual void OnRightClick(const FPointerEvent & MouseEvent) override;
	
public:



	virtual bool  Initialize() override;


	

	UFUNCTION()
	void OnHovered();

	UFUNCTION()
	void OnUnhovered();

	void OnSelected();
	void OnUnselected();


	void Init(UInventoryWidget * _inventoryWidget, int32 _index);
	void BindToStack(FItemStack* itemStack);

	bool IsEmpty() const;
	void RefreshSlot();

	FItemStack* GetItemStack() const;

	
};
