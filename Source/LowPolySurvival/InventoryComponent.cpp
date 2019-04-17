// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
#include "InventoryWidget.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	stackSlots.Init(FItemStack(), slotNum);
}


void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


bool UInventoryComponent::AddStack(FItemStack &itemstack) {

	if (!AddToExistingStacks(itemstack)) {
		if (AddToEmptySlots(itemstack)) {
			return true;
		}
	}

	return false;
}

bool UInventoryComponent::AddToExistingStacks(FItemStack &itemstack) {
	for (size_t i = 0; i < slotNum; i++) {
		if (stackSlots[i].Fill(itemstack)) {
			return true;
		}
	}

	return false;
}

bool UInventoryComponent::AddToEmptySlots(FItemStack &itemstack) {
	for (size_t i = 0; i < slotNum; ++i) {
		if (stackSlots[i].isEmpty()) {
			stackSlots[i] = itemstack;
			itemstack.Clear();
			return true;
			break;
		}

	}

	return false;
}

EInvType UInventoryComponent::GetInvType() const{
	return invType;
}

TArray<FItemStack>& UInventoryComponent::GetItemStacksRef(){
	return stackSlots;
}

