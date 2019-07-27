// Fill out your copyright notice in the Description page of Project Settings.


#include "EscapeMenuWidget.h"

FReply UEscapeMenuWidget::NativeOnPreviewKeyDown(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent) {

	if (InKeyEvent.GetKey() == EKeys::Tab || InKeyEvent.GetKey() == EKeys::Escape) {
		CloseUI();
	}

	return FReply(Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent));
}