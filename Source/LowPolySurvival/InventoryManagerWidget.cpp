// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryManagerWidget.h"
#include "InventoryWidget.h"
#include "InventoryComponent.h"
#include "ItemStackWidget.h"
#include "WidgetTree.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "QuickSlotsWidget.h"


bool UInventoryManagerWidget::Initialize(){
	Super::Initialize();

	if (WidgetTree && rootCanvas && itemStackWidget_W) {
		mouseStackWidget = WidgetTree->ConstructWidget<UItemStackWidget>(itemStackWidget_W);
		mouseStackWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		mouseStackCanvasSlot = Cast<UCanvasPanelSlot>(rootCanvas->AddChild(mouseStackWidget));
		
		mouseStackCanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		mouseStackCanvasSlot->SetSize(FVector2D(80, 80));

		//mouseStackWidget->SetRenderScale(FVector2D(1.2f, 1.2f));
	}

	mouseStack = new FItemStack();
	mouseStackWidget->Init(mouseStack);

	return true;
}

FEventReply UInventoryManagerWidget::OnPreviewKeyDown(FGeometry MyGeometry, FKeyEvent InKeyEvent) {

	if (InKeyEvent.GetKey() == EKeys::Tab || InKeyEvent.GetKey() == EKeys::E || InKeyEvent.GetKey() == EKeys::Escape) {
		bIsOtherInventoryOpen = false;
		inGameInventoryQuickWidget->Show();
		chestInv->Hide();
		chestInv->RemoveInventoryBinding();
		CloseUI();
	}

	return FEventReply(true);
}

FEventReply UInventoryManagerWidget::OnMouseMove(FGeometry MyGeometry, const FPointerEvent & MouseEvent){
		FVector2D pos = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());

		mouseStackCanvasSlot->SetPosition(pos);

	return FEventReply(true);
}

void UInventoryManagerWidget::OnShiftLeftClick(FItemStack * clickedStack, UInventoryWidget * clickedInventoryWidget){
	
	EWidgetInvType widgetInvType = clickedInventoryWidget->GetWidgetInvType();

	if (bIsOtherInventoryOpen && currentInventoryComp) {

	}

	switch (widgetInvType){
	case EWidgetInvType::OTHER:

		if (!playerInventoryComp->AddStack(*clickedStack)) {
			quickSlotInventoryComp->AddStack(*clickedStack);

		}

		break;
	case EWidgetInvType::PLAYERINV:

		if (bIsOtherInventoryOpen) {
			currentInventoryComp->AddStack(*clickedStack);
		}
		else {
			quickSlotInventoryComp->AddStack(*clickedStack);
		}

		break;
	case EWidgetInvType::QUICKSLOTS:

		currentInventoryComp->AddStack(*clickedStack);

		break;
	case EWidgetInvType::EQUIPMENT:

		currentInventoryComp->AddStack(*clickedStack);
		
		break;
	default:
		break;
	}

	mouseStackWidget->RefreshStack();
}

void UInventoryManagerWidget::Init(UInventoryComponent* _playerInventory, UInventoryComponent* _quickSlotInventory, UInventoryComponent* _equipmentInventory, UQuickSlotsWidget* _inGameQuickInventoryWidget){
	

	
	playerInventoryComp = _playerInventory;
	quickSlotInventoryComp = _quickSlotInventory;
	equipmentInventoryComp = _equipmentInventory;
	inGameInventoryQuickWidget = _inGameQuickInventoryWidget;

	playerInv->Init(this, EWidgetInvType::PLAYERINV, playerInventoryComp);
	playerQuickInv->Init(this, EWidgetInvType::QUICKSLOTS, quickSlotInventoryComp);
	playerEquipInv->Init(this, EWidgetInvType::EQUIPMENT, equipmentInventoryComp);
	chestInv->Init(this);
	
}

void UInventoryManagerWidget::ShowInventory(UInventoryComponent* inventory){
	UE_LOG(LogTemp, Warning, TEXT("Show Inventory"));


	if (!inventory || IsUIOpen()) return;

	if (inGameInventoryQuickWidget) {
		inGameInventoryQuickWidget->Hide();
	}

	currentInventoryComp = inventory;
	

	if (playerInv) {
		playerInv->Show();

		SetUserFocus(GetOwningPlayer());
	}

	switch (inventory->GetInvType()){
	case EInvType::NONE:
		break;
	case EInvType::PLAYER:

		break;
	case EInvType::CHEST:
		

		if (chestInv) {
			bIsOtherInventoryOpen = true;
			currentInvWidget = chestInv;
			chestInv->BindToInventory(inventory);
			chestInv->Show();
			
			
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
	UInventoryComponent* targetInventory = playerInventoryComp;

	if (fromPlayerInventory) {
		targetInventory = currentInventoryComp;
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
