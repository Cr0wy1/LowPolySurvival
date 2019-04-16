// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
public:

	virtual bool Initialize() override;

	void CloseUI();
	void OpenUI();

	
};
