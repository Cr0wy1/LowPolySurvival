// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item.h"
#include "InventoryComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdate, const TArray<int32>&, updatedSlots);


class UInventoryWidget;


UENUM(BlueprintType)
enum class EInvType : uint8 {
	NONE,
	PLAYER,
	CHEST,
	FURNACE,
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOWPOLYSURVIVAL_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent(const FObjectInitializer & ObjectInitializer);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	EInvType invType = EInvType::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 slotNum = 10;

	TArray<FItemStack> stackSlots;
	TArray<int32> lastUpdatedSlots;

	TMap<FItemInfo*, int32> itemAmount;

	// Called when the game starts
	virtual void BeginPlay() override;

	void AddToMap(const FItemStack &itemstack);
	void RemoveFromMap(const FItemStack &itemstack);

	bool AddToExistingStacks(FItemStack &itemstack);
	bool AddToEmptySlots(FItemStack &itemstack);

public:	

	FOnInventoryUpdate OnInventoryUpdate;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	bool AddStack(FItemStack &itemstack);
	bool AddStack(int32 slotIndex, FItemStack &itemstack, int32 amount = -1);

	void Swap(int32 slotIndex, FItemStack &itemstack);

	FItemStack PullStack(int32 slotIndex, int32 amount = -1);

	//Return how many removed
	int32 RemoveStack(int32 slotIndex, int32 amount = 1);

	//Return how many removed
	int32 RemoveStacks(int32 itemId, int32 amount = 1);

	void BroadcastOnInventoryUpdate();

	int32 CountItems(FItemInfo * itemInfo) const;
	int32 CountItems(int32 itemId) const;

	bool HasEmptySlot() const;
	EInvType GetInvType() const;
	TArray<FItemStack>& GetItemStacksRef();
	

};
