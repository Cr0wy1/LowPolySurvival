// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Item.generated.h"


class ABuildings;
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

UENUM(BlueprintType) 
enum class EDropType : uint8 {
	NONE,
	SELF,
	FROMTABLE,
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
	TSubclassOf<ABuildings> buildingTemplate_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDropType dropType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 durability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 armor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanHold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanPlace;

	bool operator==(FItemInfo &other) {
		return itemid == other.itemid;
	}
};

USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FCraftPart {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 id = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 amount = 1;

	FItemInfo* itemInfo = nullptr;
};


USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FCraftingInfo : public FTableRowBase {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCraftPart> needed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCraftPart> result;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 typeIndex;

	int32 amount;
};





USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FItemStack {

	GENERATED_BODY()

	FItemInfo* itemInfo = nullptr;
	FResource* resourceInfo = nullptr;
	int32 amount = 0;

	FItemStack();

	FItemStack(FItemInfo* _itemInfo, int32 _amount = 1);

	bool isEmpty() const;

	bool CompareIds(FItemStack &otherStack) const;

	void Clear();

	bool Fill(FItemStack &otherStack);

	void Swap(FItemStack &otherStack);

	FString ToString() const;

	//return how many Items removed
	int32 Remove(int32 itemsAmount = 1);

	void PullTo(FItemStack &otherStack, int32 itemsAmount = 1);

	void PullHalf(FItemStack &otherStack);

	FItemStack& Set(FItemStack &otherStack);

	FItemStack& Set(FItemInfo* _itemInfo, int32 _amount = 1);

	int32 GetItemId() const;

	bool IsValid() const;

	static FItemStack FromId(AActor* owner, int32 id, int32 amount = 1);

};


USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FDropInfo{

	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName itemId;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 amount;

		FItemInfo* itemInfo;
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
