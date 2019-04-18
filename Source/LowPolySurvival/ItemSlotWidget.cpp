// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemSlotWidget.h"
#include "Item.h"
#include "WidgetTree.h"
#include "Button.h"
#include "ButtonSlot.h"
#include "Image.h"
#include "ItemStackWidget.h"
#include "InventoryWidget.h"
#include "InventoryManagerWidget.h"



bool  UItemSlotWidget::Initialize() {

	Super::Initialize();

	if (WidgetTree && itemStackWidget_W && rootButton) {
		
		rootButton->OnHovered.AddDynamic(this, &UItemSlotWidget::OnHovered);
		rootButton->OnUnhovered.AddDynamic(this, &UItemSlotWidget::OnUnhovered);
		
		itemStackWidget = WidgetTree->ConstructWidget<UItemStackWidget>(itemStackWidget_W);
		

		UButtonSlot* buttonSlot = Cast<UButtonSlot>(rootButton->AddChild(itemStackWidget));

		buttonSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		buttonSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}

	return true;

}

FEventReply UItemSlotWidget::OnPreviewMouseButtonDown(FGeometry MyGeometry, const FPointerEvent & MouseEvent){

	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) {

		OnLeftClick(MouseEvent);

	}else if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton) {

		OnRightClick(MouseEvent);

	}


	return FEventReply(true);
}

void UItemSlotWidget::OnHovered(){
	itemStackWidget->SetRenderScale(FVector2D(1.2f, 1.2f));

}

void UItemSlotWidget::OnUnhovered() {
	itemStackWidget->SetRenderScale(FVector2D(1.0f, 1.0f));

}


void UItemSlotWidget::OnLeftClick(const FPointerEvent & MouseEvent){
	//UE_LOG(LogTemp, Warning, TEXT("Button with index %i clicked!"), index);
	if (inventoryManager) {

		FItemStack* mouseStack = inventoryManager->GetMouseStack();
		FItemStack* slotStack = itemStackWidget->GetItemStack();

		if (MouseEvent.IsLeftShiftDown()) {
			UE_LOG(LogTemp, Warning, TEXT("OnLeftClick SHIFT + Left Click"));
			
			inventoryManager->AddStackToInventory(slotStack, inventoryWidget->IsPlayerInventory());

		}else{

			if (mouseStack->IsValid()) {
				if (!slotStack->Fill(*mouseStack)) {
					mouseStack->Swap(*slotStack);
				}
			}else {
				if (slotStack->IsValid()) {
					mouseStack->Swap(*slotStack);
				}
			}

			inventoryManager->RefreshMouseStackWidget();

		}

	
		

	}


}

void UItemSlotWidget::OnRightClick(const FPointerEvent & MouseEvent){
	UE_LOG(LogTemp, Warning, TEXT("%s"), "OnMouseButtonDown right mouse button");

	if (inventoryManager) {
		FItemStack* mouseStack = inventoryManager->GetMouseStack();
		FItemStack* slotStack = itemStackWidget->GetItemStack();

		if (mouseStack->isEmpty()) {
			if (slotStack->IsValid()) {
				slotStack->PullHalf(*mouseStack);
			}
		}else{
			if (slotStack->isEmpty() || slotStack->GetItemId() == mouseStack->GetItemId()) {
				mouseStack->PullTo(*slotStack, 1);
			}
		}

		itemStackWidget->RefreshStack();
		inventoryManager->RefreshMouseStackWidget();
	}
}


void UItemSlotWidget::Init(UInventoryWidget * _inventoryWidget){
	inventoryWidget = _inventoryWidget;
	inventoryManager = inventoryWidget->GetInventoryManager();

}

void UItemSlotWidget::BindToStack(FItemStack * itemStack){
	if (itemStackWidget) {
		itemStackWidget->Init(itemStack);
	}

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

bool UItemSlotWidget::IsEmpty() const{
	return bIsEmpty;
}





void UItemSlotWidget::RefreshSlot(){

	itemStackWidget->RefreshStack();
}

void UItemSlotWidget::SetIndex(size_t _index){
	index = _index;
}

FItemStack * UItemSlotWidget::GetItemStack() const{
	return itemStackWidget->GetItemStack();
}
