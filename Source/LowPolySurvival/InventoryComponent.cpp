// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
#include "InventoryWidget.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent(const FObjectInitializer & ObjectInitializer){
	
	PrimaryComponentTick.bCanEverTick = false;
	
	//UE_LOG(LogTemp, Warning, TEXT("UinventoryConstructor"), PrimaryComponentTick.IsTickFunctionEnabled());

}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	PrimaryComponentTick.SetTickFunctionEnable(false);

	stackSlots.Init(FItemStack(), slotNum);
}

void UInventoryComponent::AddToMap(const FItemStack & itemstack){
	if (!itemstack.itemInfo) return;
	if (itemAmount.Contains(itemstack.itemInfo)) {
		itemAmount[itemstack.itemInfo] += itemstack.amount;
	}
	else {
		itemAmount.Add(itemstack.itemInfo, itemstack.amount);

	}
	//UE_LOG(LogTemp, Warning, TEXT("Inventory %s Count: %i"), *GetName(), itemAmount[itemstack.itemInfo]);

}

void UInventoryComponent::RemoveFromMap(const FItemStack & itemstack){

	if (itemAmount.Contains(itemstack.itemInfo)) {
		itemAmount[itemstack.itemInfo] -= itemstack.amount;
		if (itemAmount[itemstack.itemInfo] < 0) {
			itemAmount[itemstack.itemInfo] = 0;
		}
	}

}


void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//UE_LOG(LogTemp, Warning, TEXT("iventory comp ticking test"));

}


bool UInventoryComponent::AddNewStack(FItemStack itemstack){

	return AddStack(itemstack);
}

bool UInventoryComponent::AddStack(FItemStack &itemstack) {
	

	bool bReturn = false;

	if (!AddToExistingStacks(itemstack)) {
		if (AddToEmptySlots(itemstack)) {
			bReturn = true;
		}
	}
	else {
		bReturn = true;
	}
	
	BroadcastOnInventoryUpdate();
	
	return bReturn;
}

bool UInventoryComponent::AddStack(int32 slotIndex, FItemStack & itemstack, int32 amount){
	
	bool bReturn = false;

	FItemStack tempStack = itemstack;
	if (amount < 0) {
		
		bReturn = stackSlots[slotIndex].Fill(itemstack);
	}
	else {
		itemstack.PullTo(stackSlots[slotIndex], amount);
		bReturn = true;
	}
	tempStack.amount -= itemstack.amount;
	AddToMap(tempStack);

	lastUpdatedSlots.AddUnique(slotIndex);
	BroadcastOnInventoryUpdate();

	return bReturn;
}


bool UInventoryComponent::AddToExistingStacks(FItemStack &itemstack) {

	FItemStack tempStack = itemstack;

	for (size_t i = 0; i < slotNum; i++) {

		//Maybe optimize
		lastUpdatedSlots.AddUnique(i);

		if (stackSlots[i].Fill(itemstack)) {
			AddToMap(tempStack);
			return true;
		}
	}

	tempStack.amount -= itemstack.amount;
	AddToMap(tempStack);

	return false;
}

bool UInventoryComponent::AddToEmptySlots(FItemStack &itemstack) {
	for (size_t i = 0; i < slotNum; ++i) {
		if (stackSlots[i].isEmpty()) {
			stackSlots[i] = itemstack;
			AddToMap(itemstack);
			itemstack.Clear();

			lastUpdatedSlots.AddUnique(i);
			return true;
		}

	}

	return false;
}


void UInventoryComponent::Swap(int32 slotIndex, FItemStack & itemstack){
	
	stackSlots[slotIndex].Swap(itemstack);
	RemoveFromMap(itemstack);
	AddToMap(stackSlots[slotIndex]);

	lastUpdatedSlots.AddUnique(slotIndex);
	BroadcastOnInventoryUpdate();
}

FItemStack UInventoryComponent::PullStack(int32 slotIndex, int32 amount){

	amount = amount < 0 ? stackSlots[slotIndex].amount : amount;

	FItemStack tempStack = stackSlots[slotIndex];
	tempStack.amount = RemoveStack(slotIndex, amount);

	return tempStack;
}


int32 UInventoryComponent::RemoveStack(int32 slotIndex, int32 amount){
	
	FItemStack tempStack = stackSlots[slotIndex];
	tempStack.amount = stackSlots[slotIndex].Remove(amount);
	RemoveFromMap(tempStack);

	lastUpdatedSlots.AddUnique(slotIndex);
	BroadcastOnInventoryUpdate();
	
	return tempStack.amount;
}

int32 UInventoryComponent::RemoveStacks(int32 itemId, int32 amount){

	int32 removed = 0;
	for (size_t i = 0; i < stackSlots.Num(); i++){
		if (stackSlots[i].IsValid() && stackSlots[i].itemInfo->itemid == itemId) {
			removed += RemoveStack(i, amount - removed);
			
		}

		if (removed >= amount) {
			break;
		}
	}

	//BroadcastOnInventoryUpdate();

	return removed;
}


void UInventoryComponent::BroadcastOnInventoryUpdate(){
	OnInventoryUpdate.Broadcast(lastUpdatedSlots);
	lastUpdatedSlots.Empty();
	//UE_LOG(LogTemp, Warning, TEXT("adding stack"));
}


int32 UInventoryComponent::CountItems(FItemData * itemInfo) const {
	if (itemAmount.Contains(itemInfo)) {
		return itemAmount[itemInfo];
	}
	return 0;
}


int32 UInventoryComponent::CountItems(int32 itemId) const{

	int32 count = 0;

	for (auto &elem : itemAmount) {
		if (elem.Key->itemid == itemId) {
			count += elem.Value;
		}
	}

	return count;
}

bool UInventoryComponent::HasEmptySlot()const{

	for (size_t i = 0; i < stackSlots.Num(); i++){
		if (stackSlots[i].isEmpty()) {
			return true;
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



