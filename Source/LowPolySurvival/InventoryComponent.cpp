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


void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//UE_LOG(LogTemp, Warning, TEXT("iventory comp ticking test"));

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

bool UInventoryComponent::AddStack(UInventoryComponent * otherInventory, int32 otherSlotIndex){

	AddStack(otherInventory->stackSlots[otherSlotIndex]);
	
	otherInventory->lastUpdatedSlots.AddUnique(otherSlotIndex);

	otherInventory->BroadcastOnInventoryUpdate();
	

	return true;
}

bool UInventoryComponent::AddToExistingStacks(FItemStack &itemstack) {
	for (size_t i = 0; i < slotNum; i++) {

		//Maybe optimize
		lastUpdatedSlots.AddUnique(i);

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

			lastUpdatedSlots.AddUnique(i);
			return true;
		}

	}

	return false;
}

bool UInventoryComponent::AddToSlot(int32 slotIndex, FItemStack & itemstack, int32 amount){

	itemstack.PullTo(stackSlots[slotIndex], amount);
	lastUpdatedSlots.AddUnique(slotIndex);
	BroadcastOnInventoryUpdate();

	return true;
}

bool UInventoryComponent::Swap(int32 slotIndex, FItemStack & itemstack){
	stackSlots[slotIndex].Swap(itemstack);
	lastUpdatedSlots.AddUnique(slotIndex);
	BroadcastOnInventoryUpdate();

	return true;
}


bool UInventoryComponent::FillSlot(int32 slotIndex, FItemStack & itemstack){
	bool bReturn = stackSlots[slotIndex].Fill(itemstack);
	lastUpdatedSlots.AddUnique(slotIndex);
	BroadcastOnInventoryUpdate();

	return bReturn;
}

bool UInventoryComponent::TakeOffFromSlot(int32 slotIndex, int32 amount){
	stackSlots[slotIndex].TakeOff(amount);

	lastUpdatedSlots.AddUnique(slotIndex);
	BroadcastOnInventoryUpdate();

	return true;
}


void UInventoryComponent::BroadcastOnInventoryUpdate(){
	OnInventoryUpdate.Broadcast(lastUpdatedSlots);
	lastUpdatedSlots.Empty();
	//UE_LOG(LogTemp, Warning, TEXT("adding stack"));
}

bool UInventoryComponent::HasEmptySlot(){

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

