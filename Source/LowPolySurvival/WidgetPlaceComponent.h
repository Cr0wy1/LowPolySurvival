// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "WidgetPlaceComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LOWPOLYSURVIVAL_API UWidgetPlaceComponent : public UWidgetComponent
{
	GENERATED_BODY()
	


protected:

	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snapping")
	TArray<FTransform> snappingPoints;


	UWidgetPlaceComponent(const FObjectInitializer & ObjectInitializer);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	FTransform GetSnappingPoint(int32 index) const;
};
