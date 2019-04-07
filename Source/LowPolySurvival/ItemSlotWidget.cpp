// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemSlotWidget.h"
#include "Item.h"
#include "WidgetTree.h"
#include "Button.h"
#include "ButtonSlot.h"
#include "Image.h"
#include "ItemStackWidget.h"


bool  UItemSlotWidget::Initialize() {

	Super::Initialize();

	if (WidgetTree && itemStackWidget_W) {
		rootButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		itemStackWidget = WidgetTree->ConstructWidget<UItemStackWidget>(itemStackWidget_W);
		
		WidgetTree->RootWidget = rootButton;
		UPanelSlot* slot = rootButton->AddChild(itemStackWidget);
		UButtonSlot* buttonSlot = Cast<UButtonSlot>(slot);

		buttonSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		buttonSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

		UE_LOG(LogTemp, Warning, TEXT("%s"), "Hallo");
	}

	return true;

}

bool UItemSlotWidget::IsEmpty() const{
	return bIsEmpty;
}

bool UItemSlotWidget::FillStack(FItemStack * itemstack){

	if (!IsEmpty()) {
		if (itemStackWidget->itemStack->itemInfo.itemid == itemstack->itemInfo.itemid) {
			itemStackWidget->itemStack->amount += itemstack->amount;

			itemStackWidget->UpdateItemStack();
			return true;
		}
	}

	return false;
}

void UItemSlotWidget::SetItem(FItemStack* itemstack){
	UE_LOG(LogTemp, Warning, TEXT("Set Item %s in Slot"), *itemstack->itemInfo.name.ToString());
	bIsEmpty = false;

	itemStackWidget->SetNewStack(itemstack);
}
