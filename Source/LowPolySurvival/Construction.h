// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buildings.h"
#include "Construction.generated.h"


class UInventoryComponent;
/**
 * 
 */





UCLASS()
class LOWPOLYSURVIVAL_API AConstruction : public ABuildings
{
	GENERATED_BODY()
	
	
protected:
	//virtual void BeginPlay() override;




public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	UInventoryComponent * inventoryComp;


	AConstruction();

	//virtual void Tick(float deltaTime) override;

	
	virtual void Interact(ALowPolySurvivalCharacter* interactor) override;


	
};
