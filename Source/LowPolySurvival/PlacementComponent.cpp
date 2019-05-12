// Fill out your copyright notice in the Description page of Project Settings.


#include "PlacementComponent.h"
#include "Buildings.h"
#include "Engine/World.h"
#include "Materials/MaterialInstance.h"
#include "Engine/StaticMesh.h"
#include "LowPolySurvivalCharacter.h"
#include "Engine/StaticMeshSocket.h"
#include "Components/WidgetComponent.h"
#include "PlaceWidget.h"

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

		character->CrosshairLineTrace(cHitResult, cHitDirection);

		ShowPlaceWidget();

		UE_LOG(LogTemp, Warning, TEXT("%s"), *cHitResult.ImpactPoint.ToString());


		if (bObjectSnapping) {
			
			SnapToObjectSocket();

		}
		else {
			SnapToHitSurface();
			if (bGridSnapping) {
				SnapToWorldGrid();

			}
		}

		currentBuilding->AddActorLocalRotation(FRotator(0, placeRotation, 0));
	}
}

void UPlacementComponent::Init(ALowPolySurvivalCharacter * _character){
	character = _character;
}

void UPlacementComponent::ActivatePlacement(TSubclassOf<ABuildings> building_BP){
	UE_LOG(LogTemp, Warning, TEXT("Activate Placement"));
	currentBuilding_BP = building_BP;

	FHitResult hitResult;
	FVector hitDirection;
	
	character->CrosshairLineTrace(hitResult, hitDirection);

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

bool UPlacementComponent::SnapToObjectSocket(){

	ABuildings* targetBuilding = Cast<ABuildings>(cHitResult.GetActor());

	if (!targetBuilding) { return false; }

	TArray<UStaticMeshSocket*> sockets = targetBuilding->GetStaticMesh()->Sockets;
	
	if (sockets.Num() < 1) { return false; }


	FVector dir;
	float length;

	(cHitResult.Location - (targetBuilding->GetActorLocation() + sockets[0]->RelativeLocation)).ToDirectionAndLength(dir, length);

	UStaticMeshSocket* closestSocket = sockets[0];
	float closestDistance = length;

	for (size_t i = 1; i < sockets.Num(); i++) {
		(cHitResult.Location - (targetBuilding->GetActorLocation() + sockets[i]->RelativeLocation)).ToDirectionAndLength(dir, length);

		if (length < closestDistance) {
			closestDistance = length;
			closestSocket = sockets[i];
		}
		
	}

	FTransform socketTransform;
	closestSocket->GetSocketTransform(socketTransform, targetBuilding->GetStaticMeshComp());

	currentBuilding->SetActorTransform(socketTransform);

	return true;
}

bool UPlacementComponent::SnapToHitSurface(){

	FMatrix rotMatrix = FRotationMatrix::MakeFromZX(cHitResult.ImpactNormal, cHitDirection);

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *rotMatrix.Rotator().ToString());

	currentBuilding->SetActorLocation(cHitResult.ImpactPoint, false);
	currentBuilding->SetActorRotation(rotMatrix.Rotator());



	return false;
}

bool UPlacementComponent::SnapToWorldGrid(){

	FVector cLocation = currentBuilding->GetActorLocation() / worldGridSize;

	cLocation.X = FMath::RoundToInt(cLocation.X);
	cLocation.Y = FMath::RoundToInt(cLocation.Y);
	cLocation.Z = FMath::RoundToInt(cLocation.Z);

	currentBuilding->SetActorLocation(cLocation*worldGridSize);

	return true;
}

void UPlacementComponent::ShowPlaceWidget(){
	if (cHitResult.GetActor()) {

		ABuildings* targetBuilding = Cast<ABuildings>(cHitResult.GetActor());

		if (targetBuilding) {

			FMatrix rotMatrix = FRotationMatrix::MakeFromZX(cHitResult.ImpactNormal, cHitDirection);

			character->placeWidgetComp->SetWorldLocation(targetBuilding->GetActorLocation());
			character->placeWidgetComp->SetWorldRotation(targetBuilding->GetActorRotation());

			APlaceWidget* placeWidget = Cast<APlaceWidget>(character->placeWidgetComp->GetChildActor());

			if (placeWidget) {
				UE_LOG(LogTemp, Warning, TEXT("%s"), *placeWidget->GetHoveredWidgetComp()->GetName() );
			}
			

		}


		//character->placeWidgetComp->AddRelativeRotation(cHitResult.GetActor()->GetActorRotation());
	}
}

void UPlacementComponent::SetPlaceRotation(float value){
	placeRotation = value;
}

void UPlacementComponent::AddPlaceRotation(float value){
	placeRotation += value;
}

bool UPlacementComponent::IsPlacementActive() const{
	return bIsActive;
}

bool UPlacementComponent::IsRotationMode() const{
	return bIsInRotationMode;
}

void UPlacementComponent::ToggleGridSnapping(){
	bGridSnapping = !bGridSnapping;
}

void UPlacementComponent::ToggleObjectSnapping(){
	bObjectSnapping = !bObjectSnapping;
}

