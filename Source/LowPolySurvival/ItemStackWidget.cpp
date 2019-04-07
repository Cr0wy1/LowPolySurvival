// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemStackWidget.h"
#include "Image.h"
#include "TextBlock.h"
#include "Item.h"

bool  UItemStackWidget::Initialize() {

	Super::Initialize();


	return true;

}

void UItemStackWidget::UpdateItemStack(){

	if (itemImage && amountText) {
		amountText->SetText(FText::FromString(FString::FromInt(itemStack->amount)));
		itemImage->SetBrushFromTexture(itemStack->itemInfo.texture);
	}

}

void UItemStackWidget::SetNewStack(FItemStack * stack){
	itemStack = stack;

	UpdateItemStack();
}
