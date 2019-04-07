// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "ItemStackWidget.generated.h"


class UImage;
class UTextBlock;
struct FItemStack;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UItemStackWidget : public UHUDWidget
{
	GENERATED_BODY()

protected:

	

	
public:

	FItemStack * itemStack;

	virtual bool Initialize() override;

	UPROPERTY(meta = (BindWidget))
	UImage* itemImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* amountText;


	void UpdateItemStack();
	void SetNewStack(FItemStack* stack);
	
};
