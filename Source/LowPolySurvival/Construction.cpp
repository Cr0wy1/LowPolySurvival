// Fill out your copyright notice in the Description page of Project Settings.

#include "Construction.h"
#include "InventoryComponent.h"
#include "LowPolySurvivalCharacter.h"

AConstruction::AConstruction(){
	inventory = CreateDefaultSubobject<UInventoryComponent>("Inventory");
}

void AConstruction::Interact(ALowPolySurvivalCharacter * interactor){
	//inventory->AddToPlayerViewport(interactor->GetPlayerController());
	interactor->OpenInventory(inventory);
}
