// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemStackWidget.h"
#include "Image.h"
#include "TextBlock.h"
#include "Item.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture2D.h"
#include "StaticMeshResources.h"
#include "FunctionLibrary.h"
#include "GameStructs.h"


bool UItemStackWidget::Initialize() {
	Super::Initialize();

	SetVisibility(ESlateVisibility::HitTestInvisible);

	return true;
}

void UItemStackWidget::Init(FItemStack *_itemStack){
	itemStack = _itemStack;
	RefreshStack();
}

void UItemStackWidget::Init(FItemData *itemInfo, int32 amount){
	itemStack = new FItemStack(itemInfo, amount);
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
		
		//UMaterialInstance * materialInstance = Cast<UMaterialInstance>(itemStack->itemInfo->texture);

		//if (materialInstance) {
		//	UMaterialInstanceDynamic* dynMaterial = UMaterialInstanceDynamic::Create(materialInstance, this);
		//	FLinearColor color(1, 1, 1, 1);
		//	dynMaterial->SetVectorParameterByIndex(0, color);
		//	//itemStack->itemInfo->texture = dynMaterial;
		//}

		if (itemStack->itemInfo->texture) {
			UTexture2D* texture = Cast<UTexture2D>(itemStack->itemInfo->texture);

			if (texture) {

				//Set Resource color
				if (itemStack->resourceInfo) {
					texture = UFunctionLibrary::CreateColoredTexture(texture, itemStack->resourceInfo->color);
				}
				
				itemImage->SetBrushFromTexture(texture);
				
			}
			
		}


		FSlateBrush brush;
		brush.SetResourceObject(itemStack->itemInfo->texture);
		
		//itemImage->SetBrush(brush);
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

