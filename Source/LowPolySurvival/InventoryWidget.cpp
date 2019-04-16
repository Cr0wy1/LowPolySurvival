// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryWidget.h"
#include "WidgetTree.h"
#include "ItemSlotWidget.h"
#include "ItemStackWidget.h"
#include "UniformGridPanel.h"
#include "UniformGridSlot.h"
#include "Item.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "Button.h"
#include "InventoryManagerWidget.h"


bool UInventoryWidget::Initialize(){
	Super::Initialize();

	if (WidgetTree) {
		

		//grid = WidgetTree->ConstructWidget<UUniformGridPanel>();
		//UCanvasPanelSlot* canvasGridSlot = Cast<UCanvasPanelSlot>(root->AddChild(grid));
		//canvasGridSlot->SetAnchors(FAnchors(0.5f));
		//canvasGridSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		//canvasGridSlot->SetSize(FVector2D(500, 500));
		//grid->SetSlotPadding(FMargin(3.0f));


		if (itemSlotWidget_W) {

			for (size_t r = 0; r < rows; r++) {
				for (size_t c = 0; c < cols; c++) {
					slots.Add(WidgetTree->ConstructWidget<UItemSlotWidget>(itemSlotWidget_W));
					slots.Last()->SetIndex(r*cols + c);
					UUniformGridSlot* slot = Cast<UUniformGridSlot>(grid->AddChild(slots.Last()));

					slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
					slot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
					slot->Row = r;
					slot->Column = c;
				}

			}
		}

		if (itemStackWidget_W) {
			itemStackHolder = WidgetTree->ConstructWidget<UItemStackWidget>(itemStackWidget_W);
			//stackHolderSlot = Cast<UCanvasPanelSlot>(root->AddChild(itemStackHolder));

			//stackHolderSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			//stackHolderSlot->SetSize(FVector2D(100, 100));
		}

		
	}

	mouseStack = new FItemStack();

	return true;
}




void UInventoryWidget::CloseInventory(){
	RemoveFromViewport();
	APlayerController* playerController = GetOwningPlayer();
	playerController->bShowMouseCursor = false;
	playerController->SetInputMode(FInputModeGameOnly());
}

void UInventoryWidget::Init(TArray<FItemStack>& itemStacks, UInventoryManagerWidget* inventoryManager){
	for (size_t i = 0; i < itemStacks.Num(); i++){
		slots[i]->Init(inventoryManager, &itemStacks[i]);
	}

	itemStackHolder->Init(mouseStack);

}

void UInventoryWidget::MouseTakeStack(FItemStack &itemStack){

	if (itemStackHolder && itemStackHolder->GetItemStack()) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *itemStack.ToString());

	}



	itemStackHolder->GetItemStack()->Swap(itemStack);
	itemStackHolder->RefreshStack();
	bMouseIsHoldingStack = true;
}

void UInventoryWidget::Show(){
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UInventoryWidget::Hide(){
	SetVisibility(ESlateVisibility::Collapsed);
}

FItemStack * UInventoryWidget::GetMouseStack() const{

	return itemStackHolder->GetItemStack();
}




