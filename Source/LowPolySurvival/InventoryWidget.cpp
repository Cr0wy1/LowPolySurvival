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
#include "InventoryComponent.h"


bool UInventoryWidget::Initialize(){
	Super::Initialize();


	return true;
}


void UInventoryWidget::CloseInventory(){
	RemoveFromViewport();
	APlayerController* playerController = GetOwningPlayer();
	playerController->bShowMouseCursor = false;
	playerController->SetInputMode(FInputModeGameOnly());
}

void UInventoryWidget::Init(UInventoryManagerWidget* _inventoryManager, EWidgetInvType _widgetInvType, UInventoryComponent* bindInventory){

	inventoryManager = _inventoryManager;
	widgetInvType = _widgetInvType;
	bindedInventory = bindInventory;

}

void UInventoryWidget::BindToInventory(UInventoryComponent * inventoryComp){
	bindedInventory = inventoryComp;
	TArray<FItemStack>* itemStacks = &inventoryComp->GetItemStacksRef();
	for (size_t i = 0; i < slots.Num() && i < itemStacks->Num(); i++) {
		slots[i]->BindToStack(&(*itemStacks)[i]);
	}
	UE_LOG(LogTemp, Warning, TEXT("bindtoinventory"));

	bindedInventory->OnInventoryUpdate.AddDynamic(this, &UInventoryWidget::Refresh);
	//UE_LOG(LogTemp, Warning, TEXT("%i"), itemStacks->Num());
}

void UInventoryWidget::RemoveInventoryBinding(){
	if (bindedInventory) {
		bindedInventory->OnInventoryUpdate.RemoveDynamic(this, &UInventoryWidget::Refresh);
	}
	
	bindedInventory = nullptr;
}

void UInventoryWidget::Refresh(const TArray<int32> updatedSlots){

	for (size_t i = 0; i < updatedSlots.Num(); i++) {
		slots[updatedSlots[i]]->RefreshSlot();
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

EWidgetInvType UInventoryWidget::GetWidgetInvType() const{
	return widgetInvType;
}


void UInventoryWidget::AddItemSlotWidget(UItemSlotWidget * itemSlotWidget){
	slots.Add(itemSlotWidget);
}

void UInventoryWidget::AddItemSlotWidgets(const TArray<UItemSlotWidget*>& _slots){
	slots.Append(_slots);
}

void UInventoryWidget::SetItemSlotWidgetArray(const TArray<UItemSlotWidget*>& _slots){
	slots = _slots;
}

void UInventoryWidget::OnSlotArrayReady(){
	bIsSlotArrayReady = true;

	for (size_t i = 0; i < slots.Num(); i++) {
		slots[i]->Init(this, i);
	}

	if (bindedInventory) {
		
		BindToInventory(bindedInventory);
	}
}






