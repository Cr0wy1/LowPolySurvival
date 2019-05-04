// Fill out your copyright notice in the Description page of Project Settings.

#include "QuickSlotsWidget.h"
#include "ItemSlotWidget.h"





FItemStack* UQuickSlotsWidget::SelectSlot(int32 slotIndex) {

	if (slotIndex >= 0 && slotIndex < 10) {
		
		slots[currentSlotIndex]->OnUnhovered();
		slots[currentSlotIndex]->OnUnselected();

		slots[slotIndex]->OnHovered();
		slots[slotIndex]->OnSelected();
		

		currentSlotIndex = slotIndex;
	}

	return slots[currentSlotIndex]->GetItemStack();
}

FItemStack* UQuickSlotsWidget::SelectNextSlot(){
	nextSlotIndex = currentSlotIndex + 1;
	if (nextSlotIndex > 9) {
		nextSlotIndex = 0;
	}
	return SelectSlot(nextSlotIndex);
}

FItemStack* UQuickSlotsWidget::SelectPreviousSlot(){
	nextSlotIndex = currentSlotIndex - 1;
	if (nextSlotIndex < 0) {
		nextSlotIndex = 9;
	}
	return SelectSlot(nextSlotIndex);
}

int32 UQuickSlotsWidget::GetCurrentSelectedSlot() const
{
	return currentSlotIndex;
}
