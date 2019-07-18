// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayercharController.generated.h"



class UDebugWidget;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API APlayercharController : public APlayerController
{
	GENERATED_BODY()
	

protected:

	UDebugWidget * debugWidget = nullptr;

	virtual void BeginPlay() override;

	bool bIsDebugScreenShown = false;

	virtual void SetupInputComponent() override;
	void OnToggleDebugScreen();
	
	
public:

	void CenterMouse();
	
	void UpdateDebugScreen();
};
