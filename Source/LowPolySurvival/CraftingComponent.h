// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftingComponent.generated.h"



class UInventoryComponent;
class UDataTable;
struct FCraftingInfo;
struct FCraftPart;

UENUM(BlueprintType)
enum class EInvAccess : uint8 {
	INPUT,
	OUTPUT,
	BOTH,
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOWPOLYSURVIVAL_API UCraftingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCraftingComponent();

protected:

	bool bIsCrafting = false;

	UDataTable * craftingTable = nullptr;
	UDataTable * itemTable = nullptr;

	int32 craftTypeIndex = 0;

	TArray<UInventoryComponent*> inputInventories;
	TArray<UInventoryComponent*> outputInventories;

	TArray<FCraftingInfo*> craftingQueue;

	FTimerHandle craftTimerHandle;

	// Called when the game starts
	virtual void BeginPlay() override;

	void OnCraftBegin();
	void OnCraftEnd();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init(int32 _craftTypeIndex = 0);

	
	void Craft(FName craftingId, int32 amount = 1);
	void Craft(FCraftingInfo* craftingInfo, int32 amount = 1);

	void AddInventoryAccess(UInventoryComponent* inventory, EInvAccess invAccess);
	void RemoveInventoryAccess(UInventoryComponent* inventory, EInvAccess invAccess);

	void RemoveNeededParts(const TArray<FCraftPart>& neededItems, int32 amount);

	int32 CountInputItems(int32 itemId)const;

	int32 CountCraftingOperations(const TArray<FCraftPart>& neededItems);

	bool IsCrafting() const;
	bool CheckNeededItems(const TArray<FCraftPart>& neededItems ) const;
	
};
