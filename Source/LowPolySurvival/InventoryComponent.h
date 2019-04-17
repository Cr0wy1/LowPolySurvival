// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item.h"
#include "InventoryComponent.generated.h"



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

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	bool AddStack(FItemStack &itemstack);
	bool AddToExistingStacks(FItemStack &itemstack);
	bool AddToEmptySlots(FItemStack &itemstack);

	EInvType GetInvType() const;
	TArray<FItemStack>& GetItemStacksRef();

};
