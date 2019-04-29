// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemSlotWidget.h"

#include "WidgetTree.h"
#include "Button.h"
#include "ButtonSlot.h"
#include "Image.h"
#include "ItemStackWidget.h"
#include "InventoryWidget.h"
#include "InventoryManagerWidget.h"
#include "Engine/Texture2D.h"
#include "InventoryComponent.h"




bool  UItemSlotWidget::Initialize() {

	Super::Initialize();

	if (WidgetTree && rootButton) {
		
		rootButton->OnHovered.AddDynamic(this, &UItemSlotWidget::OnHovered);
		rootButton->OnUnhovered.AddDynamic(this, &UItemSlotWidget::OnUnhovered);
		
		//itemStackWidget = WidgetTree->ConstructWidget<UItemStackWidget>(itemStackWidget_W);
		

		UButtonSlot* buttonSlot = Cast<UButtonSlot>(itemStackWidget->Slot);
		buttonSlot->SetPadding(FMargin(slotPaddingOverride));
		//buttonSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		//buttonSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

		if (slotImageOverride) {
			//rootButton->WidgetStyle.Normal.SetResourceObject(UObject(FObjectInitializer()))
			
			rootButton->WidgetStyle.Normal.SetResourceObject(slotImageOverride);
			rootButton->WidgetStyle.Hovered.SetResourceObject(slotImageOverride);
			rootButton->WidgetStyle.Pressed.SetResourceObject(slotImageOverride);
			
		}
		
		normalSlateBrush = rootButton->WidgetStyle.Normal;
			
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

void UItemSlotWidget::OnSelected(){
	if (slotSelectedIcon) {
		rootButton->WidgetStyle.Normal.SetResourceObject(slotSelectedIcon);
		rootButton->WidgetStyle.Normal.TintColor = FSlateColor(FColor::White);
		
	}
	
}

void UItemSlotWidget::OnUnselected(){

	rootButton->WidgetStyle.SetNormal(normalSlateBrush);

}


void UItemSlotWidget::OnLeftClick(const FPointerEvent & MouseEvent){

	//UE_LOG(LogTemp, Warning, TEXT("%i"), index);

	
	if (inventoryManager && inventoryWidget->bindedInventory) {

		if (MouseEvent.IsLeftShiftDown()) {
			
			if (slotStack->IsValid()) {
				inventoryManager->OnShiftLeftClick(index, inventoryWidget);
				//inventoryManager->AddStackToInventory(slotStack, inventoryWidget->IsPlayerInventory());

			}
			

		}else{

			if (mouseStack->IsValid()) {

				if (limitedItemType == EItemType::NONE || limitedItemType == mouseStack->itemInfo->type) {
					if (limitedTypeIndex == 0 || mouseStack->itemInfo->typeindex == limitedTypeIndex) {

						if (!inventoryWidget->bindedInventory->FillSlot(index, *mouseStack)) {
							//UE_LOG(LogTemp, Warning, TEXT("%s"), "Hallo");

							inventoryWidget->bindedInventory->Swap(index, *mouseStack);

						}

					}
				}


			}else {
				if (slotStack->IsValid()) {

					inventoryWidget->bindedInventory->Swap(index, *mouseStack);

				}
			}

			inventoryManager->RefreshMouseStackWidget();

		}

	
		

	}


}

void UItemSlotWidget::OnRightClick(const FPointerEvent & MouseEvent){

	if (inventoryManager && inventoryWidget->bindedInventory) {


		if (mouseStack->isEmpty()) {
			if (slotStack->IsValid()) {
				slotStack->PullHalf(*mouseStack);
			}
		}else{
			if (slotStack->isEmpty() || slotStack->GetItemId() == mouseStack->GetItemId()) {

				if (limitedItemType == EItemType::NONE || limitedItemType == mouseStack->itemInfo->type) {
					if (limitedTypeIndex == 0 || mouseStack->itemInfo->typeindex == limitedTypeIndex) {

						inventoryWidget->bindedInventory->AddToSlot(index, *mouseStack, 1);

					}
				}

				
			}
		}

		itemStackWidget->RefreshStack();
		inventoryManager->RefreshMouseStackWidget();
	}
}


void UItemSlotWidget::Init(UInventoryWidget * _inventoryWidget, int32 _index){
	inventoryWidget = _inventoryWidget;
	inventoryManager = inventoryWidget->GetInventoryManager();

	index = _index;

	if (inventoryManager) {
		mouseStack = inventoryManager->GetMouseStack();
	}
	
	

}

void UItemSlotWidget::BindToStack(FItemStack * itemStack){
	if (itemStackWidget) {
		itemStackWidget->Init(itemStack);
		slotStack = itemStack;
	}

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

bool UItemSlotWidget::IsEmpty() const{
	return bIsEmpty;
}


void UItemSlotWidget::RefreshSlot(){

	itemStackWidget->RefreshStack();
}


FItemStack * UItemSlotWidget::GetItemStack() const{
	return itemStackWidget->GetItemStack();
}
