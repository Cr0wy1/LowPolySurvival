// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlotWidget.h"
#include "CraftingSlotWidget.generated.h"


class UCraftingComponent;
struct FCraftingInfo;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UCraftingSlotWidget : public USlotWidget
{
	GENERATED_BODY()

protected:

	FCraftingInfo * craftingInfo = nullptr;
	UCraftingComponent* craftingComp = nullptr;

	virtual void OnLeftClick(const FPointerEvent & MouseEvent) override;
	virtual void OnRightClick(const FPointerEvent & MouseEvent) override;

public:


	virtual bool  Initialize() override;

	
	void Init(FCraftingInfo* _craftingInfo, UCraftingComponent* _craftingComp);

};
