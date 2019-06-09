// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingComponent.h"
#include "InventoryComponent.h"
#include "Engine/DataTable.h"
#include "MyGameInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Item.h"


UCraftingComponent::UCraftingComponent(){

	PrimaryComponentTick.bCanEverTick = false;

}


void UCraftingComponent::BeginPlay(){
	Super::BeginPlay();

	craftingTable = GetWorld()->GetGameInstance<UMyGameInstance>()->GetCraftingTable();
	itemTable = GetWorld()->GetGameInstance<UMyGameInstance>()->GetItemTable();
}




void UCraftingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCraftingComponent::Init(int32 _craftTypeIndex){
	craftTypeIndex = _craftTypeIndex;
}

void UCraftingComponent::Craft(FName craftingId, int32 amount){
	if (!craftingTable) return;

	FCraftingInfo* craftingInfo = craftingTable->FindRow<FCraftingInfo>(craftingId, FString());
	Craft(craftingInfo, amount);
}

void UCraftingComponent::Craft(FCraftingInfo * craftingInfo, int32 amount){
	if (craftingInfo && craftingInfo->typeIndex == craftTypeIndex) {

		int32 operations = CountCraftingOperations(craftingInfo->needed);
		operations = amount > operations ? operations : amount;

		if (operations > 0) {

			RemoveNeededParts(craftingInfo->needed, operations);

			int32 findIndex;
			if (craftingQueue.Find(craftingInfo, findIndex)) {
				craftingQueue[findIndex]->amount += operations;
			}
			else {
				craftingInfo->amount = operations;
				craftingQueue.Push(craftingInfo);
			}

			if (!bIsCrafting) {
				OnCraftBegin();
			}
		}
	}
}

void UCraftingComponent::OnCraftBegin(){
	bIsCrafting = true;
	
	GetWorld()->GetTimerManager().SetTimer(craftTimerHandle, this, &UCraftingComponent::OnCraftEnd, craftingQueue[0]->time, false);
}

void UCraftingComponent::OnCraftEnd() {
	
	//Foreach result in crafting row
	for (const FCraftPart& craftResult : craftingQueue[0]->result){
		FItemStack itemStack;
		itemStack.itemInfo = craftResult.itemInfo;
		itemStack.amount = craftResult.amount;

		if (!itemStack.itemInfo) {
			continue;
		}

		for (UInventoryComponent* outputInventory : outputInventories) {
			if (outputInventory->AddStack(itemStack)) {
				break;
			}
		}
	}

	--craftingQueue[0]->amount;

	if (craftingQueue[0]->amount <= 0) {
		craftingQueue.RemoveAt(0, 1, true);
	}

	if (craftingQueue.Num() > 0) {
		OnCraftBegin();
	}
	else {
		bIsCrafting = false;
	}

}

void UCraftingComponent::AddInventoryAccess(UInventoryComponent * inventory, EInvAccess invAccess){
	switch (invAccess){
		case EInvAccess::INPUT:
			inputInventories.AddUnique(inventory);
			break;
		case EInvAccess::OUTPUT:
			outputInventories.AddUnique(inventory);
			break;
		default:
			inputInventories.AddUnique(inventory);
			outputInventories.AddUnique(inventory);
			break;
	}
	
}

void UCraftingComponent::RemoveInventoryAccess(UInventoryComponent * inventory, EInvAccess invAccess){
	switch (invAccess) {
		case EInvAccess::INPUT:
			inputInventories.Remove(inventory);
			break;
		case EInvAccess::OUTPUT:
			outputInventories.Remove(inventory);
			break;
		default:
			inputInventories.Remove(inventory);
			outputInventories.Remove(inventory);
			break;
	}

}

void UCraftingComponent::RemoveNeededParts(const TArray<FCraftPart>& neededItems, int32 amount){

	for (const FCraftPart& craftPart : neededItems) {
		
		int32 removed = 0;
		for (UInventoryComponent* inventory : inputInventories) {
			removed += inventory->RemoveStacks(craftPart.id, craftPart.amount * amount);
			if (removed >= craftPart.amount * amount) {
				break;
			}
		}

	}

}

int32 UCraftingComponent::CountInputItems(int32 itemId)const{
	
	int32 count = 0;

	for(UInventoryComponent* inventory : inputInventories){
		count += inventory->CountItems(itemId);
	}
	
	return count;
}

int32 UCraftingComponent::CountCraftingOperations(const TArray<FCraftPart>& neededItems){

	int32 operations = -1;

	for (const FCraftPart& craftPart : neededItems) {
		int32 partOps = CountInputItems(craftPart.id) / craftPart.amount;
		
		if (partOps < operations || operations < 0) {
			operations = partOps;
		}
	}

	return operations;
}

bool UCraftingComponent::IsCrafting() const{
	return bIsCrafting;
}

bool UCraftingComponent::CheckNeededItems(const TArray<FCraftPart>& neededItems) const{
	
	for (const FCraftPart& craftPart : neededItems) {
		if (CountInputItems(craftPart.id) < craftPart.amount) {
			return false;
		}
	}

	return true;
}

