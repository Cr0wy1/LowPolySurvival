// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlotWidget.h"
#include "CraftingSlotWidget.generated.h"


struct FCraftingInfo;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UCraftingSlotWidget : public USlotWidget
{
	GENERATED_BODY()

public:


	virtual bool  Initialize() override;

	virtual void Init() override;
	void Init(FCraftingInfo* craftingInfo);

	void Test();
};
