// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Item.generated.h"


class UTexture2D;
class UStaticMeshComponent;


USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FItemInfo : public FTableRowBase{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 itemid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* texture;

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

	bool Fill(FItemStack &otherStack) {

		if (CompareIds(otherStack)) {
			amount += otherStack.amount;
			return true;
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
