// Fill out your copyright notice in the Description page of Project Settings.


#include "PlacementComponent.h"
#include "Construction.h"
#include "Engine/World.h"
#include "Materials/MaterialInstance.h"
#include "Engine/StaticMesh.h"
#include "LowPolySurvivalCharacter.h"
#include "Engine/StaticMeshSocket.h"
#include "Components/WidgetComponent.h"
#include "PlaceWidget.h"
#include "Item.h"
#include "PlacementMenuWidget.h"
#include "PlayerHUDWidget.h"

// Sets default values for this component's properties
UPlacementComponent::UPlacementComponent(){

	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPlacementComponent::BeginPlay()
{
	Super::BeginPlay();

	placeWidget = Cast<APlaceWidget>(character->placeWidgetComp->GetChildActor());
	
}


// Called every frame
void UPlacementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (currentBuilding) {

		placeMargin = 0;

		character->CrosshairLineTrace(cHitResult, cHitDirection);
		
		ABuildings* targetBuilding = Cast<ABuildings>(cHitResult.GetActor());

		if (character->placementMenuWidget->IsObjectSnapChecked() && targetBuilding) {
			
			if (targetBuilding->HasPlaceInterface()) {
				ShowPlaceWidget();
				SnapToObjectInterface();

			}
			else {
				SnapToObjectSocket();
			}

		}
		else {

			HidePlaceWidget();

			SnapToHitSurface();

			if (character->placementMenuWidget->IsGridSnapChecked()) {
				SnapToWorldGrid();

			}
		}

		if (character->placementMenuWidget->IsLockRotChecked()) {
			placeTrans.SetRotation(FQuat(FRotator(0,0,0)));
		}

		currentBuilding->SetActorTransform(placeTrans);
		currentBuilding->AddActorLocalRotation(placeRotation);

		currentBuilding->SetMargin(placeMargin);

		if (currentBuilding->IsOverlappingBuilding()) {
			currentBuilding->SetMaterial(cannotPlaceMI);
		}
		else {
			currentBuilding->SetMaterial(canPlaceMI);
		}

		AConstruction* construction = Cast<AConstruction>(currentBuilding);

		if (construction) {
			if (character->placementMenuWidget->IsIntersectChecked()) {
				currentBuilding->AddActorLocalOffset(construction->GetIntersectLoc());
			}

			
			
		}
	}
}

void UPlacementComponent::Init(ALowPolySurvivalCharacter * _character){
	character = _character;
	
}

void UPlacementComponent::ActivatePlacement(FItemInfo* itemInfo){
	currentBuildingTemplate_BP = itemInfo->buildingTemplate_BP;

	FHitResult hitResult;
	FVector hitDirection;
	
	character->CrosshairLineTrace(hitResult, hitDirection);

	currentBuilding = GetWorld()->SpawnActor<ABuildings>(currentBuildingTemplate_BP, hitResult.Location, FRotator(0.0f) );
	currentBuilding->SetHolo(true);
	currentBuilding->ConstructFromItem(itemInfo);
	currentBuilding->SetMaterial(canPlaceMI);

	AConstruction* construction = Cast<AConstruction>(currentBuilding);
	if (construction) {
		
		construction->SetOrigin(placeOriginIndex);
		construction->GetStaticMeshComp()->AddRelativeRotation(FRotator(0, 0, placeFlipRot));
	}

	bIsActive = true;

}

void UPlacementComponent::DeactivatePlacement(){
	bIsActive = false;

	if (currentBuilding) {
		currentBuilding->SetActorHiddenInGame(true);
		currentBuilding->Destroy();
		currentBuilding = nullptr;
	}

	HidePlaceWidget();

}

bool UPlacementComponent::PlaceBuilding(){

	if (currentBuilding && currentBuildingTemplate_BP) {

		//FActorSpawnParameters params;
		//params.Template = currentBuilding;
		

		ABuildings* placedBuilding = GetWorld()->SpawnActor<ABuildings>(currentBuildingTemplate_BP, currentBuilding->GetTransform());
		placedBuilding->SetSceneTransform(currentBuilding->GetSceneTransform());
		placedBuilding->SetMeshSceneTransform(currentBuilding->GetMeshSceneTransform());
		placedBuilding->SetMeshTransform(currentBuilding->GetMeshTransform());


		placedBuilding->ConstructFromItem(currentBuilding->info.itemInfo);
		//placedBuilding->SetHolo(false);
		placedBuilding->OnPlace();
	}
	
	bIsActive = false;

	if (character->placementMenuWidget->IsResetRotChecked()) {
		placeRotation = FRotator(0, 0, 0);
	}

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

	closestSocket->GetSocketTransform(placeTrans, targetBuilding->GetStaticMeshComp());

	return true;
}

bool UPlacementComponent::SnapToObjectInterface(){

	if (placeWidget->GetHoveredWidgetIndex() >= 0) {
		if (placeWidget) {
			placeTrans = placeWidget->GetPlacementTransform(placeMargin);
			
		}
	}

	return false;
}

bool UPlacementComponent::SnapToHitSurface(){

	FMatrix rotMatrix = FRotationMatrix::MakeFromZX(cHitResult.ImpactNormal, cHitDirection);

	placeTrans = FTransform(rotMatrix.Rotator(), cHitResult.ImpactPoint);

	return false;
}

bool UPlacementComponent::SnapToWorldGrid(){

	//UE_LOG(LogTemp, Warning, TEXT("World grid snap"));

	
	FVector cLocation = cHitResult.Location / worldGridSize;

	cLocation.X = FMath::RoundToInt(cLocation.X);
	cLocation.Y = FMath::RoundToInt(cLocation.Y);
	cLocation.Z = FMath::RoundToInt(cLocation.Z);

	placeTrans.SetLocation( cLocation*worldGridSize );

	return true;
}

void UPlacementComponent::ShowPlaceWidget(){

		ABuildings* targetBuilding = Cast<ABuildings>(cHitResult.GetActor());

		if (targetBuilding && targetBuilding->HasPlaceInterface()) {

			character->placeWidgetComp->SetWorldTransform(targetBuilding->GetStaticMeshComp()->GetComponentToWorld());

			character->placeWidgetComp->SetVisibility(true);
		}

}

void UPlacementComponent::HidePlaceWidget(){
	character->placeWidgetComp->SetVisibility(false);
}

void UPlacementComponent::SetPlaceRotation(float value){
	placeRotation.Yaw = value;
}

void UPlacementComponent::AddPlaceRotation(float valueY, float valueZ){

	placeRotation.Pitch += valueY;
	placeRotation.Yaw += valueZ;
	
}

bool UPlacementComponent::IsPlacementActive() const{
	return bIsActive;
}

bool UPlacementComponent::IsRotationMode() const{
	return bIsInRotationMode;
}

void UPlacementComponent::ToggleGridSnapping(){
	character->placementMenuWidget->ToggleGridSnap();
}

void UPlacementComponent::ToggleObjectSnapping(){
	character->placementMenuWidget->ToggleObjectSnap();
}

void UPlacementComponent::ToggleIntersect(){
	character->placementMenuWidget->ToggleIntersect();
}

void UPlacementComponent::SwitchOrigin(){
	AConstruction* construction = Cast<AConstruction>(currentBuilding);
	if (construction) {
		placeOriginIndex = construction->SetNextOrigin();
		construction->GetStaticMeshComp()->AddRelativeRotation(FRotator(0, 0, placeFlipRot));
	}
}

void UPlacementComponent::OnRPressed(){

	placeFlipRot += 90;
	
	AConstruction* construction = Cast<AConstruction>(currentBuilding);
	if (construction) {
		construction->GetStaticMeshComp()->AddRelativeRotation(FRotator(0, 0, 90));
	}
}

