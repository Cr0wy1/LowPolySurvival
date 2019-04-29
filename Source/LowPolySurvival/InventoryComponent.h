// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item.h"
#include "InventoryComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdate, const TArray<int32>, updatedSlots);


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
	UInventoryComponent();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	EInvType invType = EInvType::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 slotNum = 10;

	TArray<FItemStack> stackSlots;
	TArray<int32> lastUpdatedSlots;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	FOnInventoryUpdate OnInventoryUpdate;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	bool AddStack(FItemStack &itemstack);
	bool AddStack(UInventoryComponent* otherInventory, int32 otherSlotIndex);
	bool AddToExistingStacks(FItemStack &itemstack);
	bool AddToEmptySlots(FItemStack &itemstack);

	bool AddToSlot(int32 slotIndex, FItemStack &itemstack, int32 amount);
	bool Swap(int32 slotIndex, FItemStack &itemstack);
	bool FillSlot(int32 slotIndex, FItemStack &itemstack);
	//bool PullFromSlotTo(int32 slotIndex, FItemStack &targetStack)

	void BroadcastOnInventoryUpdate();

	bool HasEmptySlot();
	EInvType GetInvType() const;
	TArray<FItemStack>& GetItemStacksRef();

};
