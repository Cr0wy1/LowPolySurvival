// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buildings.h"
#include "Flame.generated.h"



class UParticleSystemComponent;
class UPointLightComponent;

/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API AFlame : public ABuildings
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Particles")
	UParticleSystemComponent* flameParticleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Light")
	UPointLightComponent* flameLightComp;

public:

	AFlame();

	UFUNCTION(BlueprintCallable)
	void ActivateFlame();

	UFUNCTION(BlueprintCallable)
	void DeactivateFlame();
	
	
};
