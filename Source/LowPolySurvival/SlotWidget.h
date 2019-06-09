// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "SlotWidget.generated.h"


class UButton;
class UItemStackWidget;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API USlotWidget : public UHUDWidget
{
	GENERATED_BODY()


protected:

	UPROPERTY(meta = (BindWidget))
	UButton* rootButton;

	UPROPERTY(meta = (BindWidget))
	UItemStackWidget* itemStackWidget = nullptr;

	virtual void OnLeftClick(const FPointerEvent & MouseEvent);
	virtual void OnRightClick(const FPointerEvent & MouseEvent);

public:

	virtual void Init();

	FReply NativeOnPreviewMouseButtonDown(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent);

};
