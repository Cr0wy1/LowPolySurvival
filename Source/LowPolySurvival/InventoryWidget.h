// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "InventoryWidget.generated.h"


class UItem;
class UItemSlotWidget;
class UItemStackWidget;
struct FItemInfo;
struct FItemStack;
class UUniformGridPanel;
class UCanvasPanel;
class UCanvasPanelSlot;
class UButton;
class UInventoryManagerWidget;
class UInventoryComponent;

/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UInventoryWidget : public UHUDWidget
{
	GENERATED_BODY()


protected:

	bool bIsPlayerInventory = false;
	bool bIsSlotArrayReady = false;
	UInventoryManagerWidget* inventoryManager;
	UInventoryComponent* bindedInventory = nullptr;
	
	TArray<UItemSlotWidget*> slots;


public:


	virtual bool  Initialize() override;

	virtual void NativeTick(const FGeometry & MyGeometry, float InDeltaTime) override;


	void CloseInventory();
	void Init(UInventoryManagerWidget* _inventoryManager, bool isPlayerInventory = false, UInventoryComponent* bindInventory = nullptr);
	void BindToInventory(UInventoryComponent* inventoryComp);
	
	void Refresh();

	void Show();
	void Hide();

	UInventoryManagerWidget* GetInventoryManager() const;
	bool IsPlayerInventory()const;

	UFUNCTION(BlueprintCallable)
	void AddItemSlotWidget(UItemSlotWidget* itemSlotWidget);

	UFUNCTION(BlueprintCallable)
	void AddItemSlotWidgets(const TArray<UItemSlotWidget*> &_slots);

	UFUNCTION(BlueprintCallable)
	void SetItemSlotWidgetArray(const TArray<UItemSlotWidget*> &_slots);

	UFUNCTION(BlueprintCallable)
	void OnSlotArrayReady();
	
};
