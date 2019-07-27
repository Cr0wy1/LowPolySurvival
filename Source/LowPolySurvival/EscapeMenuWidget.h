// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIWidget.h"
#include "EscapeMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UEscapeMenuWidget : public UUIWidget
{
	GENERATED_BODY()
	

public:

	FReply NativeOnPreviewKeyDown(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent) override;
};
