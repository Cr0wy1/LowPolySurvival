// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlaceWidget.generated.h"


class UStaticMeshComponent;
class UStaticMeshSocket;
class UWidgetPlaceComponent;
class UArrowComponent;


USTRUCT()
struct LOWPOLYSURVIVAL_API FSnappingSurface {

	GENERATED_BODY()
		 

	UWidgetPlaceComponent* widgetPlaceComp;

	TArray<FName> socketNames;
	TArray<int32> margins;
};


UCLASS()
class LOWPOLYSURVIVAL_API APlaceWidget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlaceWidget();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Template Mesh")
	UStaticMeshComponent* meshTemplate;

	TArray<FSnappingSurface> snappingSurfaces;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void UpdateAllWidgetComps();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int32 GetHoveredWidgetIndex() const;
	FTransform GetPlacementTransform(int32 &margin) const;
	
};
