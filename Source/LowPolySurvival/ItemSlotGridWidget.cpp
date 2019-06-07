// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemSlotGridWidget.h"
#include "SlotWidget.h"
#include "WidgetTree.h"
#include "UniformGridPanel.h"
#include "UniformGridSlot.h"



bool UItemSlotGridWidget::Initialize() {
	Super::Initialize();

	if (WidgetTree) {
		if (bGenerateInventory) {

			for (size_t i = 0; i < numOfSlots; ++i){
				AddSlot();
			}

		}
	}

	return true;
}

void UItemSlotGridWidget::AddSlot(){
	if (itemSlotWidget_W && grid) {
		slots.Add(WidgetTree->ConstructWidget<USlotWidget>(itemSlotWidget_W));
		
		UUniformGridSlot* slot = Cast<UUniformGridSlot>(grid->AddChild(slots.Last()));

		slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		slot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

		slot->Row = (slots.Num() - 1) / cols;
		slot->Column = (slots.Num() - 1) % cols;
	}
}

TArray<USlotWidget*>& UItemSlotGridWidget::GetSlotsRef()
{
	return slots;
}
