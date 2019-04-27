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

	bool bReturn = false;

	if (!AddToExistingStacks(itemstack)) {
		if (AddToEmptySlots(itemstack)) {
			bReturn = true;
		}
	}
	
	BroadcastOnInventoryUpdate();
	
	return bReturn;
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

bool UInventoryComponent::AddToSlot(int32 slotIndex, FItemStack & itemstack, int32 amount){

	itemstack.PullTo(stackSlots[slotIndex], amount);
	BroadcastOnInventoryUpdate();

	return true;
}

bool UInventoryComponent::Swap(int32 slotIndex, FItemStack & itemstack){
	stackSlots[slotIndex].Swap(itemstack);
	BroadcastOnInventoryUpdate();

	return true;
}

bool UInventoryComponent::Swap(int32 slotIndex, UInventoryComponent * otherInventory, int32 otherSlotIndex){
	Swap(slotIndex, otherInventory->stackSlots[otherSlotIndex]);
	otherInventory->BroadcastOnInventoryUpdate();
	BroadcastOnInventoryUpdate();

	return true;
}

bool UInventoryComponent::FillSlot(int32 slotIndex, FItemStack & itemstack){
	bool bReturn = stackSlots[slotIndex].Fill(itemstack);
	BroadcastOnInventoryUpdate();

	return bReturn;
}


void UInventoryComponent::BroadcastOnInventoryUpdate(){
	OnInventoryUpdate.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("adding stack"));
}

EInvType UInventoryComponent::GetInvType() const{
	return invType;
}

TArray<FItemStack>& UInventoryComponent::GetItemStacksRef(){
	return stackSlots;
}

