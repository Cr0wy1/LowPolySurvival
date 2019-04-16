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


void UInventoryComponent::AddStack(FItemStack &itemstack) {

	if (!AddToExistingStacks(itemstack)) {
		AddToEmptySlots(itemstack);
	}

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *stackSlots[0].ToString() );
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *stackSlots[1].ToString());

}

bool UInventoryComponent::AddToExistingStacks(FItemStack &itemstack) {
	for (size_t i = 0; i < slotNum; i++) {
		if (stackSlots[i].Fill(itemstack)) {
			return true;
		}
	}

	return false;
}

void UInventoryComponent::AddToEmptySlots(FItemStack &itemstack) {
	for (size_t i = 0; i < slotNum; ++i) {
		if (stackSlots[i].isEmpty()) {
			stackSlots[i] = itemstack;
			break;
		}

	}
}

EInvType UInventoryComponent::GetInvType() const{
	return invType;
}

TArray<FItemStack>& UInventoryComponent::GetItemStacksRef(){
	return stackSlots;
}

