// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "ItemSlotWidget.generated.h"


class UButton;
class UImage;
struct FItemInfo;
struct FItemStack;
class UItemStackWidget;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UItemSlotWidget : public UHUDWidget
{
	GENERATED_BODY()
	

protected:
	bool bIsEmpty = true;

	UButton* rootButton = nullptr;
	UItemStackWidget* itemStackWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Stack")
	TSubclassOf<UItemStackWidget> itemStackWidget_W;
	
public:



	virtual bool  Initialize() override;

	//UPROPERTY(meta = (BindWidget))
	//UButton* slotButton;

	//UPROPERTY(meta = (BindWidget))
	//UImage* slotImage;

	bool IsEmpty() const;

	bool FillStack(FItemStack* itemstack);

	void SetItem(FItemStack* itemstack);

	


	
};
