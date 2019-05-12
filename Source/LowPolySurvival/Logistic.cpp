// Fill out your copyright notice in the Description page of Project Settings.

#include "Logistic.h"
#include "InventoryComponent.h"
#include "LowPolySurvivalCharacter.h"


ALogistic::ALogistic() {
	inventoryComp = CreateDefaultSubobject<UInventoryComponent>("Inventory");
}

void ALogistic::Interact(ALowPolySurvivalCharacter * interactor) {
	Super::Interact(interactor);

	interactor->OpenInventory(this);
}

