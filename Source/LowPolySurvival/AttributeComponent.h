// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FAttributes {

	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 health = 100;

	UPROPERTY(EditAnywhere)
	int32 healthMax = 100;

	UPROPERTY(EditAnywhere)
	int32 stamina = 100;

	UPROPERTY(EditAnywhere)
	int32 staminaMax = 100;

	UPROPERTY(EditAnywhere)
	float staminaRegDuration = 1.0f;

	UPROPERTY(EditAnywhere)
	float moveSpeed = 200;

	UPROPERTY(EditAnywhere)
	int32 dashCost = 300;

	UPROPERTY(EditAnywhere)
	float hitRange = 200.0f;
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOWPOLYSURVIVAL_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent(const FObjectInitializer & ObjectInitializer);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	FAttributes attributes;

	float lastStaminaReloadSeconds = 0;

	// Called when the game starts
	virtual void BeginPlay() override;

	void UpdateStamina();
	void ChargeHealth(int32 amount);
	void ChargeStamina(int32 amount);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//void ApplyConsumeBuff(FConsumeBuff buff);


	void RemoveHealth(int32 amount);
	void RemoveStamina(int32 amount);

	FAttributes GetAttributes() const;
	FAttributes& GetAttributesRef();
	
};
