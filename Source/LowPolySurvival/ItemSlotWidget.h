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
	FItemStack* slotStack;

	FSlateBrush normalSlateBrush;

	UPROPERTY(meta = (BindWidget))
	UButton* rootButton;

	UPROPERTY(meta = (BindWidget))
	UItemStackWidget* itemStackWidget = nullptr;

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

	void OnLeftClick(const FPointerEvent & MouseEvent);
	void OnRightClick(const FPointerEvent & MouseEvent);
	
public:



	virtual bool  Initialize() override;


	FReply NativeOnPreviewMouseButtonDown(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent) override;

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
