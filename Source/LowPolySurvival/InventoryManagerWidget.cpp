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
		stackHolder = WidgetTree->ConstructWidget<UItemStackWidget>(itemStackWidget_W);
		stackHolderSlot = Cast<UCanvasPanelSlot>(rootCanvas->AddChild(stackHolder));
		
		stackHolderSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		stackHolderSlot->SetSize(FVector2D(100, 100));
	}

	mouseStack = new FItemStack();
	stackHolder->Init(mouseStack);

	return true;
}

FEventReply UInventoryManagerWidget::OnPreviewKeyDown(FGeometry MyGeometry, FKeyEvent InKeyEvent) {
	UE_LOG(LogTemp, Warning, TEXT("%s"), "Hallo");

	if (InKeyEvent.GetKey() == EKeys::Tab || InKeyEvent.GetKey() == EKeys::E || InKeyEvent.GetKey() == EKeys::Escape) {
		chestInv->Hide();
		CloseUI();
	}
	else {

	}

	return FEventReply(true);
}

FEventReply UInventoryManagerWidget::OnMouseMove(FGeometry MyGeometry, const FPointerEvent & MouseEvent){
	if (bMouseIsHoldingStack) {
		FVector2D pos = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());

		stackHolderSlot->SetPosition(pos);

		UE_LOG(LogTemp, Warning, TEXT("%s"), *pos.ToString());
	}

	return FEventReply(true);
}

void UInventoryManagerWidget::Init(UInventoryComponent * _playerInventory){
	playerInventory = _playerInventory;


}

void UInventoryManagerWidget::ShowInventory(UInventoryComponent* inventory){
	if (!inventory) return;

	if (playerInv) {
		playerInv->Show();
		playerInv->Init(playerInventory->GetItemStacksRef(), this);
		SetUserFocus(GetOwningPlayer());
	}

	switch (inventory->GetInvType()){
	case EInvType::NONE:
		break;
	case EInvType::PLAYER:

		break;
	case EInvType::CHEST:
		

		if (chestInv) {
			chestInv->Show();
			chestInv->Init(inventory->GetItemStacksRef(), this);
			
		}

		break;
	case EInvType::FURNACE:
		break;
	default:
		break;
	}

	OpenUI();
}

void UInventoryManagerWidget::MouseTakeStack(FItemStack &itemStack) {

	if (stackHolder && stackHolder->GetItemStack()) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *itemStack.ToString());

		stackHolder->GetItemStack()->Swap(itemStack);
		stackHolder->RefreshStack();
		bMouseIsHoldingStack = true;

	}
}
