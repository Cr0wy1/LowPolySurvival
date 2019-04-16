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

	//UPROPERTY(meta = (BindWidget))
	//UCanvasPanel* root;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel *grid;

public:

	bool bMouseIsHoldingStack = false;

	virtual bool  Initialize() override;


	void CloseInventory();
	void Init(TArray<FItemStack> &itemStacks, UInventoryManagerWidget* inventoryManager);
	void MouseTakeStack(FItemStack &itemStack);

	void Show();
	void Hide();

	FItemStack* GetMouseStack() const;
	
};
