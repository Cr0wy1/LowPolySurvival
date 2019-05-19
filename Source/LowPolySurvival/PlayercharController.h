// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayercharController.generated.h"

/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API APlayercharController : public APlayerController
{
	GENERATED_BODY()
	
	
public:

	void CenterMouse();
	
};
