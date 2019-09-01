// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameStructs.h"
#include "PlacementComponent.generated.h"


class ABuildings;
class AConstruction;
class ALowPolySurvivalCharacter;
class UMaterialInstance;
class APlaceWidget;
struct FItemData;


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

	float placeFlipRot = 0;
	int32 placeOriginIndex = 0;

	int32 placeMargin = 0;

	const FCrosshairResult* crosshairResult;

	ALowPolySurvivalCharacter* character = nullptr;
	APlaceWidget* placeWidget = nullptr;

	ABuildings* currentBuilding = nullptr;
	TSubclassOf<ABuildings> currentBuildingTemplate_BP;

	//UPROPERTIES
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInterface* canPlaceMI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInterface* cannotPlaceMI;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	



	float worldGridSize = 100.0f;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init(ALowPolySurvivalCharacter* _character);

		
	void ActivatePlacement(FItemData* itemInfo);
	void DeactivatePlacement();

	//return true if Building is Placed
	bool PlaceBuilding();

	//return true if snapped
	bool SnapToObjectSocket();
	bool SnapToObjectInterface();
	bool SnapToHitSurface();
	bool SnapToWorldGrid();

	void ShowPlaceWidget();
	void HidePlaceWidget();

	void SetPlaceRotation(float value);
	void AddPlaceRotation(float valueY, float valueZ);

	bool IsPlacementActive() const;
	bool IsRotationMode() const;

	void ToggleGridSnapping();
	void ToggleObjectSnapping();
	void ToggleIntersect();
	void SwitchOrigin();

	void OnRPressed();
};
