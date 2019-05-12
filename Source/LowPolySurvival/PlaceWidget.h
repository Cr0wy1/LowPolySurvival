// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlaceWidget.generated.h"



class UWidgetComponent;
class UArrowComponent;



USTRUCT()
struct LOWPOLYSURVIVAL_API FWidgetArrows {

	GENERATED_BODY()

	UWidgetComponent* widgetComp;
	TArray<UArrowComponent* > arrowComps;

};



UCLASS()
class LOWPOLYSURVIVAL_API APlaceWidget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlaceWidget();

protected:

	TArray<FWidgetArrows> widgetArrowsStructs;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void UpdateAllWidgetComps();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UWidgetComponent* GetHoveredWidgetComp() const;
	int32 GetHoveredWidgetIndex() const;

	FTransform GetPlacementTransform() const;
	
};
