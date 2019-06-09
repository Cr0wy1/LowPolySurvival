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


void UCraftingComponent::BeginPlay()
{
	Super::BeginPlay();

	craftingTable = GetWorld()->GetGameInstance<UMyGameInstance>()->GetCraftingTable();
	itemTable = GetWorld()->GetGameInstance<UMyGameInstance>()->GetItemTable();
}




void UCraftingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCraftingComponent::Init(int32 _craftTypeIndex){
	craftTypeIndex = _craftTypeIndex;
}

void UCraftingComponent::Craft(FName craftingId, int32 amount){
	if (!craftingTable) return;

	FString contextString;
	FCraftingInfo* craftingInfo = craftingTable->FindRow<FCraftingInfo>(craftingId, contextString);

	//UE_LOG(LogTemp, Warning, TEXT("Context String: %s"), *contextString);


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
	
	//UE_LOG(LogTemp, Warning, TEXT("begin crafting"));


	GetWorld()->GetTimerManager().SetTimer(craftTimerHandle, this, &UCraftingComponent::OnCraftEnd, craftingQueue[0]->time, false);
}

void UCraftingComponent::OnCraftEnd() {
	
	//Foreach result in crafting row
	for (const FCraftPart& craftResult : craftingQueue[0]->result){
		FItemStack itemStack;
		itemStack.itemInfo = itemTable->FindRow<FItemInfo>(FName(*FString::FromInt(craftResult.id)), FString());
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


	//UE_LOG(LogTemp, Warning, TEXT("Crafted Item: %i"), craftingQueue[0]->id);


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
	
	//UE_LOG(LogTemp, Warning, TEXT("inventoreis: %i"), inputInventories.Num());

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

	//UE_LOG(LogTemp, Warning, TEXT("inventoreis: %i"), inputInventories.Num());
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
	
	//UE_LOG(LogTemp, Warning, TEXT("Crafting Count: %i"), count);

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

	UE_LOG(LogTemp, Warning, TEXT("Crafting operations: %i"), operations);

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

