// Fill out your copyright notice in the Description page of Project Settings.

#include "Construction.h"
#include "LowPolySurvivalCharacter.h"

AConstruction::AConstruction(){
	
	bHasPlaceInterface = true;
}

void AConstruction::Interact(ALowPolySurvivalCharacter * interactor){
	Super::Interact(interactor);

}

void AConstruction::RotateMeshX(float value){
	subSceneComp->AddLocalRotation(FRotator(0.0f, 0.0f, value));
}

void AConstruction::RotateMeshY(float value){
	subSceneComp->AddLocalRotation(FRotator(value, 0.0f, 0.0f));
}

void AConstruction::RotateMeshZ(float value){
	subSceneComp->AddLocalRotation(FRotator(0.0f, value, 0.0f));
}

void AConstruction::RotateActorX(float value){
	AddActorLocalRotation(FRotator(0.0f, 0.0f, value));
}

void AConstruction::RotateActorY(float value){
	AddActorLocalRotation(FRotator(value, 0.0f, 0.0f));
}

void AConstruction::RotateActorZ(float value){
	AddActorLocalRotation(FRotator(0.0f, value, 0.0f));
}


FVector AConstruction::GetIntersectLoc() const{
	return intersectLoc;
}

