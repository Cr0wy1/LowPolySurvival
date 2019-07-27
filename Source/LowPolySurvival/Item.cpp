// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"
#include "Components/StaticMeshComponent.h"
#include "Buildings.h"
#include "MyGameInstance.h"
#include "GameStructs.h"


FItemStack::FItemStack() {

}

FItemStack::FItemStack(FItemInfo* _itemInfo, int32 _amount) : itemInfo(_itemInfo), amount(_amount) {
	
}

bool FItemStack::isEmpty() const {
	return (amount <= 0);
}

bool FItemStack::CompareIds(FItemStack &otherStack) const {
	return itemInfo == otherStack.itemInfo;
}

void FItemStack::Clear() {
	itemInfo = nullptr;
	amount = 0;
}

bool FItemStack::Fill(FItemStack &otherStack) {

	if (CompareIds(otherStack) && itemInfo) {

		int32 tempAmount = amount + otherStack.amount;

		if (tempAmount > itemInfo->stacksize) {
			amount = itemInfo->stacksize;
			otherStack.amount = tempAmount - itemInfo->stacksize;

			return false;
		}
		else {
			amount = tempAmount;
			otherStack.Clear();

			return true;
		}

	}

	return false;
}

void FItemStack::Swap(FItemStack &otherStack) {
	FItemStack tempStack;
	tempStack = *this;
	*this = otherStack;
	otherStack = tempStack;
}

FString FItemStack::ToString() const {
	FString strId = itemInfo ? FString::FromInt(itemInfo->itemid) : "nullptr";
	FString str = "ItemStack(id:" + strId + ", amount:" + FString::FromInt(amount) + ")";
	return str;
}

//return how many Items removed
int32 FItemStack::Remove(int32 itemsAmount) {

	amount -= itemsAmount;

	if (amount < 1) {
		itemsAmount += amount;
		Clear();
	}

	return itemsAmount;
}

void FItemStack::PullTo(FItemStack &otherStack, int32 itemsAmount) {
	//if (GetItemId() == otherStack.GetItemId()) {
	if (otherStack.isEmpty() || otherStack.amount < otherStack.itemInfo->stacksize) {

		if (itemsAmount > amount) {
			itemsAmount = amount;
		}
		amount -= itemsAmount;
		otherStack.amount += itemsAmount;
		otherStack.itemInfo = itemInfo;

		if (amount < 1) {
			Clear();
		}
	}
}

void FItemStack::PullHalf(FItemStack &otherStack) {
	int32 half = FMath::CeilToInt((float)amount / 2.0f);
	PullTo(otherStack, half);
}

FItemStack& FItemStack::Set(FItemStack &otherStack) {
	itemInfo = otherStack.itemInfo;
	amount = otherStack.amount;
	return *this;
}

FItemStack& FItemStack::Set(FItemInfo* _itemInfo, int32 _amount) {
	itemInfo = _itemInfo;
	amount = _amount;
	return *this;
}

int32 FItemStack::GetItemId() const {
	return itemInfo ? itemInfo->itemid : 0;
}

bool FItemStack::IsValid() const {
	return itemInfo && amount > 0;
}

FItemStack FItemStack::FromId(AActor* owner, int32 id, int32 amount){

	FItemStack stack;

	if (owner) {
		UMyGameInstance* gameInstance = Cast<UMyGameInstance>(owner->GetGameInstance());
		stack.itemInfo = gameInstance->GetItemTable()->FindRow<FItemInfo>(*FString::FromInt(id), FString());
		stack.resourceInfo = gameInstance->GetResourceTable()->FindRow<FResource>(*FString::FromInt(1), FString());
		stack.amount = amount;
	}

	return stack;
}



// Sets default values
UItem::UItem(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	

}



FItemInfo UItem::GetItemInfo() const{

	return itemInfo;
}

