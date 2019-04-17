// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryManagerWidget.h"
#include "InventoryWidget.h"
#include "InventoryComponent.h"
#include "ItemStackWidget.h"
#include "WidgetTree.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"


bool UInventoryManagerWidget::Initialize(){
	Super::Initialize();

	if (WidgetTree && rootCanvas && itemStackWidget_W) {
		mouseStackWidget = WidgetTree->ConstructWidget<UItemStackWidget>(itemStackWidget_W);
		mouseStackCanvasSlot = Cast<UCanvasPanelSlot>(rootCanvas->AddChild(mouseStackWidget));
		
		mouseStackCanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		mouseStackCanvasSlot->SetSize(FVector2D(100, 100));
	}

	mouseStack = new FItemStack();
	mouseStackWidget->Init(mouseStack);

	return true;
}

FEventReply UInventoryManagerWidget::OnPreviewKeyDown(FGeometry MyGeometry, FKeyEvent InKeyEvent) {

	if (InKeyEvent.GetKey() == EKeys::Tab || InKeyEvent.GetKey() == EKeys::E || InKeyEvent.GetKey() == EKeys::Escape) {
		chestInv->Hide();
		CloseUI();
	}

	return FEventReply(true);
}

FEventReply UInventoryManagerWidget::OnMouseMove(FGeometry MyGeometry, const FPointerEvent & MouseEvent){
		FVector2D pos = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());

		mouseStackCanvasSlot->SetPosition(pos);

		//UE_LOG(LogTemp, Warning, TEXT("%s"), *pos.ToString());

	return FEventReply(true);
}

void UInventoryManagerWidget::Init(UInventoryComponent * _playerInventory){
	playerInventory = _playerInventory;


}

void UInventoryManagerWidget::ShowInventory(UInventoryComponent* inventory){
	if (!inventory) return;

	currentInventory = inventory;
	

	if (playerInv) {
		playerInv->Show();
		playerInv->Init(playerInventory->GetItemStacksRef(), this, true);
		SetUserFocus(GetOwningPlayer());
	}

	switch (inventory->GetInvType()){
	case EInvType::NONE:
		break;
	case EInvType::PLAYER:

		break;
	case EInvType::CHEST:
		

		if (chestInv) {
			currentInvWidget = chestInv;
			chestInv->Show();
			chestInv->Init(inventory->GetItemStacksRef(), this, false);
			
		}

		break;
	case EInvType::FURNACE:
		break;
	default:
		break;
	}

	OpenUI();
}



void UInventoryManagerWidget::RefreshMouseStackWidget(){
	mouseStackWidget->RefreshStack();
}

bool UInventoryManagerWidget::AddStackToInventory(FItemStack* targetStack, bool fromPlayerInventory) {

	bool bIsAdded = false;
	UInventoryComponent* targetInventory = playerInventory;

	if (fromPlayerInventory) {
		targetInventory = currentInventory;
	}

	bIsAdded = targetInventory->AddStack(*targetStack);
	mouseStackWidget->RefreshStack();
		
	return bIsAdded;
}

	

bool UInventoryManagerWidget::IsHoldingStack() const{
	return mouseStackWidget->GetItemStack()->IsValid();
}

FItemStack * UInventoryManagerWidget::GetMouseStack() const{

	return mouseStack;
}
