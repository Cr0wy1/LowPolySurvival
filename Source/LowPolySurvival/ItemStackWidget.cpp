// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemStackWidget.h"
#include "Image.h"
#include "TextBlock.h"
#include "Item.h"

bool UItemStackWidget::Initialize() {
	Super::Initialize();

	SetVisibility(ESlateVisibility::HitTestInvisible);

	return true;
}

void UItemStackWidget::Init(FItemStack *_itemStack){
	itemStack = _itemStack;
	RefreshStack();
}

void UItemStackWidget::RefreshStack(){

	if (!itemImage || !amountText) return;

	if (itemStack && itemStack->IsValid()) {
		
		if (itemStack->amount < 2) {
			amountText->SetText(FText::FromString(""));
		}
		else {
			amountText->SetText(FText::FromString(FString::FromInt(itemStack->amount)));
		}
		
		
		itemImage->SetBrushFromTexture(itemStack->itemInfo->texture);
		itemImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));

	}else {
		amountText->SetText(FText::FromString(""));
		itemImage->SetColorAndOpacity(FLinearColor::Transparent);
	}

}



void UItemStackWidget::SetItemStack(FItemStack & _itemStack){
	*itemStack = _itemStack;
}

FItemStack * const UItemStackWidget::GetItemStack() const{
	return itemStack;
}
