// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryWidget.h"
#include "WidgetTree.h"
#include "ItemSlotWidget.h"
#include "UniformGridPanel.h"
#include "UniformGridSlot.h"
#include "Item.h"


bool UInventoryWidget::Initialize(){
	Super::Initialize();

	if (WidgetTree) {
		

		
		UUniformGridPanel* root = Cast<UUniformGridPanel>(GetRootWidget());

		if (itemSlotWidget_W) {
			for (size_t r = 0; r < rows; r++) {
				for (size_t c = 0; c < cols; c++) {
					slots.Add(WidgetTree->ConstructWidget<UItemSlotWidget>(itemSlotWidget_W));
					UUniformGridSlot* slot = Cast<UUniformGridSlot>(root->AddChild(slots.Last()));

					slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
					slot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
					slot->Row = r;
					slot->Column = c;
				}
			}
		}


		UE_LOG(LogTemp, Warning, TEXT("%s"), "Hallo");
	}

	return true;
}

void UInventoryWidget::AddStack(FItemStack* itemstack) {

	if (!AddToExistingStacks(itemstack)) {
		AddToEmptySlots(itemstack);
	}
	
	
}

bool UInventoryWidget::AddToExistingStacks(FItemStack * itemstack){
	for (size_t i = 0; i < slots.Num(); i++){
		if (slots[i]->FillStack(itemstack)) {
			return true;
		}
	}

	return false;
}

void UInventoryWidget::AddToEmptySlots(FItemStack * itemstack){
	for (size_t i = 0; i < slots.Num(); ++i) {
		if (slots[i]->IsEmpty()) {
			slots[i]->SetItem(itemstack);
			break;
		}
	}
}



