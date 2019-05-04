// Fill out your copyright notice in the Description page of Project Settings.


#include "PlacementComponent.h"
#include "Buildings.h"
#include "Engine/World.h"
#include "Materials/MaterialInstance.h"
#include "Engine/StaticMesh.h"
#include "LowPolySurvivalCharacter.h"

// Sets default values for this component's properties
UPlacementComponent::UPlacementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
}


// Called when the game starts
void UPlacementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPlacementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (currentBuilding) {
		FHitResult hitResult = character->CrosshairLineTrace();

		currentBuilding->SetActorLocation(hitResult.Location, false);
	}
}

void UPlacementComponent::Init(ALowPolySurvivalCharacter * _character){
	character = _character;
}

void UPlacementComponent::ActivatePlacement(TSubclassOf<ABuildings> building_BP){
	UE_LOG(LogTemp, Warning, TEXT("Activate Placement"));
	currentBuilding_BP = building_BP;

	FHitResult hitResult = character->CrosshairLineTrace();

	currentBuilding = GetWorld()->SpawnActor<ABuildings>(building_BP, hitResult.Location, FRotator(0.0f) );
	//currentBuilding->SetActorEnableCollision(false);
	//currentBuilding->SetCollisionEnabled(ECollisionEnabled::QueryOnly, true);
	currentBuilding->SetHolo(true);

	bIsActive = true;

	
}

void UPlacementComponent::DeactivatePlacement(){
	bIsActive = false;

	if (currentBuilding) {
		currentBuilding->SetActorHiddenInGame(true);
		currentBuilding->Destroy();
		currentBuilding = nullptr;
	}

}

bool UPlacementComponent::PlaceBuilding(){

	if (currentBuilding && currentBuilding_BP) {

		FActorSpawnParameters params;
		params.Template = currentBuilding;

		ABuildings* placedBuilding = GetWorld()->SpawnActor<ABuildings>(currentBuilding_BP, params);
		//placedBuilding->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics, false);
		//placedBuilding->SetActorEnableCollision(true);
		placedBuilding->SetHolo(false);
	}
	
	bIsActive = false;

	return true;
}

bool UPlacementComponent::IsPlacementActive() const{
	return bIsActive;
}

