// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIWidget.h"
#include "InventoryManagerWidget.generated.h"


class UInventoryWidget;
class UInventoryComponent;



enum class EInvType : uint8;
class UItemStackWidget;
class UCanvasPanelSlot;
class UCanvasPanel;
class UQuickSlotsWidget;
struct FItemStack;

/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UInventoryManagerWidget : public UUIWidget
{
	GENERATED_BODY()
	
	
protected:

	//Stack for mouse slot
	FItemStack * mouseStack;

	bool bMouseIsHoldingStack = false;
	//bool bIsShiftHolding = false;
	UCanvasPanelSlot* mouseStackCanvasSlot;
	UItemStackWidget* mouseStackWidget;

	UInventoryComponent* playerInventoryComp;
	UInventoryComponent* quickSlotInventoryComp;
	UInventoryComponent* equipmentInventoryComp;

	UInventoryComponent* currentInventoryComp;
	UInventoryWidget* currentInvWidget;

	UQuickSlotsWidget* inGameInventoryQuickWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Widget")
	TSubclassOf<UItemStackWidget> itemStackWidget_W;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* rootCanvas;

	UPROPERTY(meta = (BindWidget))
	UInventoryWidget* playerInv;

	UPROPERTY(meta = (BindWidget))
	UInventoryWidget* playerQuickInv;

	UPROPERTY(meta = (BindWidget))
	UInventoryWidget* playerEquipInv;

	UPROPERTY(meta = (BindWidget))
	UInventoryWidget* chestInv;

	




public:

	virtual bool  Initialize() override;

	UFUNCTION()
	FEventReply OnPreviewKeyDown(FGeometry MyGeometry, FKeyEvent InKeyEvent);

	UFUNCTION()
	FEventReply OnMouseMove(FGeometry MyGeometry, const FPointerEvent & MouseEvent);

	void Init(UInventoryComponent* _playerInventory, UInventoryComponent* _quickSlotInventory, UInventoryComponent* _equipmentInventory, UQuickSlotsWidget* _inGameQuickInventoryWidget = nullptr);
	void ShowInventory(UInventoryComponent* inventory);
	void RefreshMouseStackWidget();
	bool AddStackToInventory(FItemStack* targetStack, bool fromPlayerInventory = true);

	bool IsHoldingStack() const;
	FItemStack* GetMouseStack() const;
	
};
