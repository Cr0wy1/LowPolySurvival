// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GravityMovementComponent.generated.h"




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOWPOLYSURVIVAL_API UGravityMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	FVector gravityCenter;

	// Sets default values for this component's properties
	UGravityMovementComponent();

protected:

	UPrimitiveComponent * primitiveComp = nullptr;

	bool bIsInGravitySphere = false;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Gravity")
	void SetAffectedComp(UPrimitiveComponent* _primitiveComp);

	void SetGravity(FVector _gravityCenter);
	void RemoveGravity();
};
