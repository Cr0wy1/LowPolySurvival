// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryManagerWidget.h"
#include "InventoryWidget.h"
#include "InventoryComponent.h"
#include "ItemStackWidget.h"
#include "WidgetTree.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "QuickSlotsWidget.h"
#include "LowPolySurvivalCharacter.h"
#include "CraftingWidget.h"


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


FReply UInventoryManagerWidget::NativeOnPreviewKeyDown(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent){

	if (InKeyEvent.GetKey() == EKeys::Tab || InKeyEvent.GetKey() == EKeys::E || InKeyEvent.GetKey() == EKeys::Escape) {
		CloseInventory();
	}

	return FReply(Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent));
}


FReply UInventoryManagerWidget::NativeOnMouseMove(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent){
	FVector2D pos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	mouseStackCanvasSlot->SetPosition(pos);

	return FReply(Super::NativeOnMouseMove(InGeometry, InMouseEvent));
}

void UInventoryManagerWidget::OnShiftLeftClick(int32 clickedBtnIndex, UInventoryWidget * clickedInventoryWidget){
	
	EWidgetInvType widgetInvType = clickedInventoryWidget->GetWidgetInvType();
	UInventoryComponent* targetInventoryComp = playerInventoryComp;

	switch (widgetInvType){
	case EWidgetInvType::OTHER:

		if (playerInventoryComp->HasEmptySlot()) {
			targetInventoryComp = playerInventoryComp;
		}
		else {
			targetInventoryComp = quickSlotInventoryComp;
		}

		break;
	case EWidgetInvType::PLAYERINV:

		if (bIsOtherInventoryOpen) {
			targetInventoryComp = currentInventoryComp;
		}
		else {
			targetInventoryComp = quickSlotInventoryComp;
		}

		break;
	case EWidgetInvType::QUICKSLOTS:

		targetInventoryComp = currentInventoryComp;

		break;
	case EWidgetInvType::EQUIPMENT:

		targetInventoryComp = currentInventoryComp;
		
		break;
	default:
		break;
	}
	FItemStack clickedStack = clickedInventoryWidget->bindedInventory->PullStack(clickedBtnIndex);
	targetInventoryComp->AddStack(clickedStack);
	mouseStackWidget->RefreshStack();
}

void UInventoryManagerWidget::Init(ALowPolySurvivalCharacter* _playerCharacter, UQuickSlotsWidget* _inGameQuickInventoryWidget){
	
	playerCharacter = _playerCharacter;
	
	playerInventoryComp = playerCharacter->inventoryComp;
	quickSlotInventoryComp = playerCharacter->quickInventoryComp;
	equipmentInventoryComp = playerCharacter->equipmentInventoryComp;
	inGameInventoryQuickWidget = _inGameQuickInventoryWidget;

	playerInv->Init(this, EWidgetInvType::PLAYERINV, playerInventoryComp);
	playerQuickInv->Init(this, EWidgetInvType::QUICKSLOTS, quickSlotInventoryComp);
	playerEquipInv->Init(this, EWidgetInvType::EQUIPMENT, equipmentInventoryComp);
	chestInv->Init(this);
	
	craftingWidget->Init(playerCharacter->craftingComp);
}

void UInventoryManagerWidget::ShowInventory(UInventoryComponent* inventory){
	


	if (!inventory || IsUIOpen()) return;
	//UE_LOG(LogTemp, Warning, TEXT("Show Inventory"));
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

	OpenInventory();
}



void UInventoryManagerWidget::RefreshMouseStackWidget(){
	mouseStackWidget->RefreshStack();

	
}

void UInventoryManagerWidget::OpenInventory(){
	OpenUI();

	playerCharacter->OnInventoryOpen();
}

void UInventoryManagerWidget::CloseInventory(){
	bIsOtherInventoryOpen = false;
	inGameInventoryQuickWidget->Show();
	chestInv->Hide();
	chestInv->RemoveInventoryBinding();
	CloseUI();

	playerCharacter->OnInventoryClose();
}


bool UInventoryManagerWidget::IsHoldingStack() const{
	return mouseStackWidget->GetItemStack()->IsValid();
}

FItemStack * UInventoryManagerWidget::GetMouseStack() const{

	return mouseStack;
}
