// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemSlotGridWidget.h"
#include "ItemSlotWidget.h"
#include "WidgetTree.h"
#include "UniformGridPanel.h"
#include "UniformGridSlot.h"



bool UItemSlotGridWidget::Initialize() {
	Super::Initialize();

	if (WidgetTree) {

		if (bGenerateInventory) {
			if (itemSlotWidget_W && grid) {

				for (size_t r = 0; r < rows; r++) {
					for (size_t c = 0; c < cols; c++) {
						slots.Add(WidgetTree->ConstructWidget<UItemSlotWidget>(itemSlotWidget_W));
						
						UUniformGridSlot* slot = Cast<UUniformGridSlot>(grid->AddChild(slots.Last()));

						slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
						slot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
						slot->Row = r;
						slot->Column = c;
					}

				}
			}
		}

	}

	UE_LOG(LogTemp, Warning, TEXT("Check"));



	return true;
}

TArray<UItemSlotWidget*>& UItemSlotGridWidget::GetSlotsRef()
{
	return slots;
}
