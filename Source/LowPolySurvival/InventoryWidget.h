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



UENUM()
enum class EWidgetInvType : uint8 {
	OTHER,
	PLAYERINV,
	QUICKSLOTS,
	EQUIPMENT,
};
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UInventoryWidget : public UHUDWidget
{
	GENERATED_BODY()


protected:

	bool bIsSlotArrayReady = false;
	UInventoryManagerWidget* inventoryManager;
	
	
	TArray<UItemSlotWidget*> slots;

	EWidgetInvType widgetInvType = EWidgetInvType::OTHER;

public:

	UInventoryComponent * bindedInventory = nullptr;

	virtual bool  Initialize() override;


	void CloseInventory();
	void Init(UInventoryManagerWidget* _inventoryManager, EWidgetInvType _widgetInvType = EWidgetInvType::OTHER, UInventoryComponent* bindInventory = nullptr);
	void BindToInventory(UInventoryComponent* inventoryComp);
	void RemoveInventoryBinding();
	
	UFUNCTION()
	void Refresh();

	void Show();
	void Hide();

	UInventoryManagerWidget* GetInventoryManager() const;
	EWidgetInvType GetWidgetInvType() const;

	UFUNCTION(BlueprintCallable)
	void AddItemSlotWidget(UItemSlotWidget* itemSlotWidget);

	UFUNCTION(BlueprintCallable)
	void AddItemSlotWidgets(const TArray<UItemSlotWidget*> &_slots);

	UFUNCTION(BlueprintCallable)
	void SetItemSlotWidgetArray(const TArray<UItemSlotWidget*> &_slots);

	UFUNCTION(BlueprintCallable)
	void OnSlotArrayReady();
	
};
