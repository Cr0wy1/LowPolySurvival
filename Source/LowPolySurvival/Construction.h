// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buildings.h"
#include "Construction.generated.h"



/**
 * 
 */





UCLASS()
class LOWPOLYSURVIVAL_API AConstruction : public ABuildings
{
	GENERATED_BODY()
	
	
protected:
	//virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placement")
	FVector intersectLoc;


public:

	AConstruction();

	//virtual void Tick(float deltaTime) override;

	
	virtual void Interact(ALowPolySurvivalCharacter* interactor) override;


	void RotateMeshX(float value = 90.0f);
	void RotateMeshY(float value = 90.0f);
	void RotateMeshZ(float value = 90.0f);

	void RotateActorX(float value = 90.0f);
	void RotateActorY(float value = 90.0f);
	void RotateActorZ(float value = 90.0f);


	FVector GetIntersectLoc() const;

};
