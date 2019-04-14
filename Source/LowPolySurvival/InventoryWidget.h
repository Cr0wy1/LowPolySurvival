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
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UInventoryWidget : public UHUDWidget
{
	GENERATED_BODY()


protected:

	//Stack for mouse slot
	FItemStack * mouseStack;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Widget")
	TSubclassOf<UItemSlotWidget> itemSlotWidget_W;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Widget")
	TSubclassOf<UItemStackWidget> itemStackWidget_W;

	UItemStackWidget* itemStackHolder;
	UCanvasPanelSlot* stackHolderSlot;
	
	TArray<UItemSlotWidget*> slots;


	UPROPERTY(EditAnywhere)
	int32 rows = 4;

	UPROPERTY(EditAnywhere)
	int32 cols = 4;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* root;

	UUniformGridPanel *grid;
	UButton* backButton;

public:

	bool bMouseIsHoldingStack = false;

	virtual bool  Initialize() override;


	UFUNCTION()
	FEventReply OnPreviewKeyDown(FGeometry MyGeometry, FKeyEvent InKeyEvent);

	UFUNCTION()
	FEventReply OnMouseMove(FGeometry MyGeometry, const FPointerEvent & MouseEvent);

	UFUNCTION()
	FEventReply	OnMouseButtonDown(FGeometry MyGeometry, const FPointerEvent & MouseEvent);

	UFUNCTION()
		void OnDragDetected
		(
			FGeometry MyGeometry,
			const FPointerEvent & PointerEvent,
			UDragDropOperation *& Operation
		);

	void CloseInventory();
	void Init(TArray<FItemStack> &itemStacks);
	void MouseTakeStack(FItemStack &itemStack);

	FItemStack* GetMouseStack() const;
	
};
