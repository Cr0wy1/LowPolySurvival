// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
protected:

	APlayerController * playerController = nullptr;

	

public:

	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

	
	
};
