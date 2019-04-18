// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "ItemSlotWidget.generated.h"


class UButton;
class UImage;
struct FItemInfo;
struct FItemStack;
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

	size_t index = 0;

	UPROPERTY(meta = (BindWidget))
	UButton* rootButton;
	UItemStackWidget* itemStackWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Stack")
	TSubclassOf<UItemStackWidget> itemStackWidget_W;


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
	void SetIndex(size_t _index);

	FItemStack* GetItemStack() const;

	
};
