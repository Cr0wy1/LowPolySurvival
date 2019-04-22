// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Item.generated.h"


class UTexture2D;
class UStaticMeshComponent;


UENUM(BlueprintType)
enum class EItemType : uint8 {
	NONE,
	BLOCK,
	ORE,
	CONSUMABLE,
	TOOL,
	ARMOR,
};


UENUM(BlueprintType)
enum class EArmorType : uint8 {
	NONE,
	HELM,
	CHEST,
	LEGS,
	BOOTS,
	CHAIN,
	RING,
	BELT,
};


UENUM(BlueprintType)
enum class EToolType : uint8 {
	PICKAXE,
	AXE,
	SHOVEL,
};


USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FMetaItemInfo : public FTableRowBase {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 itemid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 typeindex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 durability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 armor;

};


USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FItemInfo : public FTableRowBase{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 itemid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 stacksize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 typeindex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 durability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 armor;

};





USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FItemStack {

	GENERATED_BODY()

	FItemInfo* itemInfo = nullptr;
	int32 amount = 0;



	bool isEmpty() {
		return (amount <= 0);
	}

	bool CompareIds(FItemStack &otherStack) {
		return GetItemId() == otherStack.GetItemId();
	}

	void Clear() {
		itemInfo = nullptr;
		amount = 0;
	}

	bool Fill(FItemStack &otherStack) {

		if (CompareIds(otherStack)) {

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

	void Swap(FItemStack &otherStack) {
		FItemStack tempStack;
		tempStack = *this;
		*this = otherStack;
		otherStack = tempStack;
	}

	FString ToString() const{
		FString strId = itemInfo ? FString::FromInt(itemInfo->itemid) : "nullptr";
		FString str = "ItemStack(id:" + strId + ", amount:" + FString::FromInt(amount) + ")";
		return str;
	}

	void PullTo(FItemStack &otherStack, int32 itemsAmount = 1) {
		//if (GetItemId() == otherStack.GetItemId()) {
			if (itemsAmount > amount) {
				itemsAmount = amount;
			}
			amount -= itemsAmount;
			otherStack.amount += itemsAmount;
			otherStack.itemInfo = itemInfo;

			if (amount < 1) {
				Clear();
			}
		//}
	}

	void PullHalf(FItemStack &otherStack) {
		int32 half = FMath::CeilToInt((float)amount / 2.0f);
		PullTo(otherStack, half);
	}

	FItemStack& Set(FItemStack &otherStack) {
		itemInfo = otherStack.itemInfo;
		amount = otherStack.amount;
		return *this;
	}

	FItemStack& Set(FItemInfo* _itemInfo, int32 _amount = 1) {
		itemInfo = _itemInfo;
		amount = _amount;
		return *this;
	}

	int32 GetItemId() const {
		return itemInfo ? itemInfo->itemid : 0;
	}

	bool IsValid() const {
		return itemInfo && amount > 0;
	}

};


USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FDropInfo{

	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName itemId;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 amount;

};




USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FItemDrops : public FTableRowBase {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDropInfo> itemId;


};




UCLASS()
class LOWPOLYSURVIVAL_API UItem : public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UItem();


protected:

	//Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FItemInfo itemInfo;


public:	
	
	FItemInfo GetItemInfo()const;
	
};
