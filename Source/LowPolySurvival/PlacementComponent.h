// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlacementComponent.generated.h"


class ABuildings;
class AConstruction;
class ALowPolySurvivalCharacter;
class UMaterialInstance;
class APlaceWidget;
struct FItemInfo;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOWPOLYSURVIVAL_API UPlacementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlacementComponent();

protected:

	bool bIsActive = false;
	bool bIsInRotationMode = false;

	FTransform placeTrans;
	FVector placeLoc;
	FRotator placeRotation;

	FHitResult cHitResult;
	FVector cHitDirection;

	ALowPolySurvivalCharacter* character = nullptr;
	APlaceWidget* placeWidget = nullptr;

	ABuildings* currentBuilding = nullptr;
	TSubclassOf<ABuildings> currentBuildingTemplate_BP;

	//UPROPERTIES
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInstance* canPlaceMI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInstance* cannotPlaceMI;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	



	float worldGridSize = 200.0f;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init(ALowPolySurvivalCharacter* _character);

		
	void ActivatePlacement(FItemInfo* itemInfo);
	void DeactivatePlacement();

	//return true if Building is Placed
	bool PlaceBuilding();

	//return true if snapped
	bool SnapToObjectSocket();
	bool SnapToObjectInterface();
	bool SnapToHitSurface();
	bool SnapToWorldGrid();

	void ShowPlaceWidget();

	void SetPlaceRotation(float value);
	void AddPlaceRotation(float valueY, float valueZ);

	bool IsPlacementActive() const;
	bool IsRotationMode() const;

	void ToggleGridSnapping();
	void ToggleObjectSnapping();
	void ToggleIntersect();

	void OnRPressed();
};
