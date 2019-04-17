// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryWidget.h"
#include "WidgetTree.h"
#include "ItemSlotWidget.h"
#include "ItemStackWidget.h"
#include "UniformGridPanel.h"
#include "UniformGridSlot.h"
#include "Item.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "Button.h"
#include "InventoryManagerWidget.h"


bool UInventoryWidget::Initialize(){
	Super::Initialize();

	if (WidgetTree) {

		if (itemSlotWidget_W && grid) {

			for (size_t r = 0; r < rows; r++) {
				for (size_t c = 0; c < cols; c++) {
					slots.Add(WidgetTree->ConstructWidget<UItemSlotWidget>(itemSlotWidget_W));
					slots.Last()->SetIndex(r*cols + c);
					UUniformGridSlot* slot = Cast<UUniformGridSlot>(grid->AddChild(slots.Last()));

					slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
					slot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
					slot->Row = r;
					slot->Column = c;
				}

			}
		}
	
	}

	UE_LOG(LogTemp, Warning, TEXT("Check"));



	return true;
}

void UInventoryWidget::NativeTick(const FGeometry & MyGeometry, float InDeltaTime){
	//UE_LOG(LogTemp, Warning, TEXT("ticking"));
	Refresh();
}



void UInventoryWidget::CloseInventory(){
	RemoveFromViewport();
	APlayerController* playerController = GetOwningPlayer();
	playerController->bShowMouseCursor = false;
	playerController->SetInputMode(FInputModeGameOnly());
}

void UInventoryWidget::Init(TArray<FItemStack>& itemStacks, UInventoryManagerWidget* _inventoryManager, bool isPlayerInventory){

	inventoryManager = _inventoryManager;
	bIsPlayerInventory = isPlayerInventory;

	for (size_t i = 0; i < itemStacks.Num(); i++){
		slots[i]->Init(this, &itemStacks[i]);
	}


}

void UInventoryWidget::Refresh(){
	for (size_t i = 0; i < slots.Num(); i++) {
		slots[i]->RefreshSlot();
	}
}


void UInventoryWidget::Show(){
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UInventoryWidget::Hide(){
	SetVisibility(ESlateVisibility::Collapsed);
}

UInventoryManagerWidget * UInventoryWidget::GetInventoryManager() const{

	return inventoryManager;
}

bool UInventoryWidget::IsPlayerInventory() const
{
	return bIsPlayerInventory;
}





