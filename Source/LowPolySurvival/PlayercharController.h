// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayercharController.generated.h"



class UMyGameInstance;
class UDebugWidget;
class UEscapeMenuWidget;
class UChatWidget;
class UMapWidget;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API APlayercharController : public APlayerController
{
	GENERATED_BODY()
	

protected:

	UMyGameInstance * gameInstance = nullptr;

	UDebugWidget * debugWidget = nullptr;
	UEscapeMenuWidget* escapeMenuWidget = nullptr;
	UChatWidget* chatWidget = nullptr;
	UMapWidget* mapWidget = nullptr;

	virtual void BeginPlay() override;

	bool bIsDebugScreenShown = false;

	virtual void SetupInputComponent() override;
	void OnToggleDebugScreen();
	void OnEscapeMenuPressed();
	void OnOpenChatPressed();
	void OnOpenMapPressed();
	
public:

	void CenterMouse();
	
	void UpdateDebugScreen();
};
