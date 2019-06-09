// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingSlotWidget.h"
#include "Item.h"
#include "ItemStackWidget.h"
#include "Engine/DataTable.h"


bool UCraftingSlotWidget::Initialize() {


	return Super::Initialize();
}

void UCraftingSlotWidget::Init(){

}

void UCraftingSlotWidget::Init(FCraftingInfo* craftingInfo) {

	//itemStackWidget->Init()

	
}

void UCraftingSlotWidget::Test(){
	UE_LOG(LogTemp, Warning, TEXT("Timer test"));

}


