// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Construction.h"
#include "Logistic.generated.h"



class UInventoryComponent;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API ALogistic : public AConstruction
{
	GENERATED_BODY()
	

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	UInventoryComponent * inventoryComp;


	ALogistic();

	//virtual void Tick(float deltaTime) override;


	virtual void Interact(ALowPolySurvivalCharacter* interactor) override;
	
};
