// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "ItemSlotGridWidget.generated.h"


class USlotWidget;
class UUniformGridPanel;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UItemSlotGridWidget : public UHUDWidget
{
	GENERATED_BODY()

protected:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Widget")
	TSubclassOf<USlotWidget> itemSlotWidget_W;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generated Inventory")
	bool bGenerateInventory = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generated Inventory")
	int32 numOfSlots = 16;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generated Inventory")
	int32 rows = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generated Inventory")
	int32 cols = 4;

	//UPROPERTY(meta = (BindWidget))
	//UCanvasPanel* root;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel *grid;

public:

	TArray<USlotWidget*> slots;

	virtual bool  Initialize() override;
	
	USlotWidget* AddSlot();

	UFUNCTION(BlueprintCallable)
	TArray<USlotWidget*> &GetSlotsRef();
};
