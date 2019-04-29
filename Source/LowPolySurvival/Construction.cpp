// Fill out your copyright notice in the Description page of Project Settings.

#include "Construction.h"
#include "InventoryComponent.h"
#include "LowPolySurvivalCharacter.h"

AConstruction::AConstruction(){
	inventoryComp = CreateDefaultSubobject<UInventoryComponent>("Inventory");
}

void AConstruction::Interact(ALowPolySurvivalCharacter * interactor){
	Super::Interact(interactor);

	interactor->OpenInventory(this);
}
