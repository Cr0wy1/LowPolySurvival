// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIWidget.h"
#include "InventoryManagerWidget.generated.h"


class ALowPolySurvivalCharacter;
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

	ALowPolySurvivalCharacter * playerCharacter = nullptr;

	//Stack for mouse slot
	FItemStack * mouseStack;

	bool bMouseIsHoldingStack = false;
	bool bIsOtherInventoryOpen = false;
	
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

	
	FReply NativeOnPreviewKeyDown(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent) override;
	
	FReply NativeOnMouseMove(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent) override;

	void OnShiftLeftClick(int32 clickedBtnIndex, UInventoryWidget* clickedInventoryWidget);

	void Init(ALowPolySurvivalCharacter* _playerCharacter, UQuickSlotsWidget* _inGameQuickInventoryWidget = nullptr);
	void ShowInventory(UInventoryComponent* inventory);
	void RefreshMouseStackWidget();

	void OpenInventory();
	void CloseInventory();

	bool IsHoldingStack() const;
	FItemStack* GetMouseStack() const;
	
};
