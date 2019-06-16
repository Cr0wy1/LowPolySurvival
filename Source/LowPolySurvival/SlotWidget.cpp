// Fill out your copyright notice in the Description page of Project Settings.


#include "SlotWidget.h"
#include "Button.h"


FReply USlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent)
{

	//UE_LOG(LogTemp, Warning, TEXT("%s"), "Mouse down");


	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) {

		OnLeftClick(InMouseEvent);

	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton) {

		OnRightClick(InMouseEvent);

	}


	return FReply(Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent));
}


void USlotWidget::OnLeftClick(const FPointerEvent & MouseEvent)
{
}

void USlotWidget::OnRightClick(const FPointerEvent & MouseEvent)
{
}
