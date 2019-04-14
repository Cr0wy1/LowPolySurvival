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


bool UInventoryWidget::Initialize(){
	Super::Initialize();

	if (WidgetTree && root) {
		
		backButton = WidgetTree->ConstructWidget<UButton>();
		backButton->SetBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.1f));
		backButton->WidgetStyle.Hovered.TintColor = FSlateColor::UseForeground();
		UCanvasPanelSlot* canvasBtnSlot = Cast<UCanvasPanelSlot>(root->AddChild(backButton));
		canvasBtnSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		canvasBtnSlot->SetOffsets(FMargin(0.0f));

		grid = WidgetTree->ConstructWidget<UUniformGridPanel>();
		UCanvasPanelSlot* canvasGridSlot = Cast<UCanvasPanelSlot>(root->AddChild(grid));
		canvasGridSlot->SetAnchors(FAnchors(0.5f));
		canvasGridSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		canvasGridSlot->SetSize(FVector2D(500, 500));
		grid->SetSlotPadding(FMargin(3.0f));


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
			stackHolderSlot = Cast<UCanvasPanelSlot>(root->AddChild(itemStackHolder));

			stackHolderSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			stackHolderSlot->SetSize(FVector2D(100, 100));
		}

		
	}

	mouseStack = new FItemStack();

	return true;
}


FEventReply UInventoryWidget::OnPreviewKeyDown(FGeometry MyGeometry, FKeyEvent InKeyEvent){

	if (InKeyEvent.GetKey() == EKeys::Tab || InKeyEvent.GetKey() == EKeys::E || InKeyEvent.GetKey() == EKeys::Escape) {
		CloseInventory();
	}
	else {

	}

	return FEventReply(true);
}

FEventReply UInventoryWidget::OnMouseMove(FGeometry MyGeometry, const FPointerEvent & MouseEvent){

	if (bMouseIsHoldingStack) {
		FVector2D pos = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());

		stackHolderSlot->SetPosition(pos);

		//UE_LOG(LogTemp, Warning, TEXT("%s"), *pos.ToString());
	}
	
	return FEventReply(true);
}

FEventReply UInventoryWidget::OnMouseButtonDown(FGeometry MyGeometry, const FPointerEvent & MouseEvent){

	UE_LOG(LogTemp, Warning, TEXT("%s"), "mousedown");


	return FEventReply(true);
}

void UInventoryWidget::OnDragDetected(FGeometry MyGeometry, const FPointerEvent & PointerEvent, UDragDropOperation *& Operation){
	UE_LOG(LogTemp, Warning, TEXT("%s"), "drag");

}

void UInventoryWidget::CloseInventory(){
	RemoveFromViewport();
	APlayerController* playerController = GetOwningPlayer();
	playerController->bShowMouseCursor = false;
	playerController->SetInputMode(FInputModeGameOnly());
}

void UInventoryWidget::Init(TArray<FItemStack>& itemStacks){
	for (size_t i = 0; i < itemStacks.Num(); i++){
		slots[i]->Init(this, &itemStacks[i]);
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

FItemStack * UInventoryWidget::GetMouseStack() const{

	return itemStackHolder->GetItemStack();
}




