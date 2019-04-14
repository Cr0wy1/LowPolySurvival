// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemSlotWidget.h"
#include "Item.h"
#include "WidgetTree.h"
#include "Button.h"
#include "ButtonSlot.h"
#include "Image.h"
#include "ItemStackWidget.h"
#include "InventoryWidget.h"



bool  UItemSlotWidget::Initialize() {

	Super::Initialize();

	if (WidgetTree && itemStackWidget_W && rootButton) {
		
		rootButton->OnPressed.AddDynamic(this, &UItemSlotWidget::OnPressed);
		itemStackWidget = WidgetTree->ConstructWidget<UItemStackWidget>(itemStackWidget_W);
		

		UButtonSlot* buttonSlot = Cast<UButtonSlot>(rootButton->AddChild(itemStackWidget));

		buttonSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		buttonSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}

	return true;

}


void UItemSlotWidget::OnPressed(){
	UE_LOG(LogTemp, Warning, TEXT("Button with index %i clicked!"), index);
	if (inventory) {

		inventory->MouseTakeStack(*itemStackWidget->GetItemStack());

		itemStackWidget->RefreshStack();

	}

	rootButton->OnReleased.Broadcast();
	rootButton->OnClicked.Broadcast();

}


void UItemSlotWidget::Init(UInventoryWidget * _inventory, FItemStack* itemStack){
	inventory = _inventory;

	if (itemStackWidget) {
		itemStackWidget->Init(itemStack);
	}
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
