// Fill out your copyright notice in the Description page of Project Settings.

#include "Construction.h"
#include "LowPolySurvivalCharacter.h"
#include "Engine/StaticMesh.h"

AConstruction::AConstruction(){
	
	bHasPlaceInterface = true;

	origins.Add(FTransform(FRotator(90.0f, 0, 0), FVector(0,0,-50.0f)));

}

void AConstruction::Interact(ALowPolySurvivalCharacter * interactor){
	Super::Interact(interactor);

}

void AConstruction::RotateMeshX(float value){
	meshSceneComp->AddLocalRotation(FRotator(0.0f, 0.0f, value));
}

void AConstruction::RotateMeshY(float value){
	meshSceneComp->AddLocalRotation(FRotator(value, 0.0f, 0.0f));
}

void AConstruction::RotateMeshZ(float value){
	meshSceneComp->AddLocalRotation(FRotator(0.0f, value, 0.0f));
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

void AConstruction::SetNextOrigin(){
	++cOriginIndex;
	if (cOriginIndex < 0 || cOriginIndex >= origins.Num()) {
		meshComp->SetRelativeTransform(FTransform());
		cOriginIndex = -1;
	}
	else {
		meshComp->SetRelativeTransform(origins[cOriginIndex]);
	}
	UE_LOG(LogTemp, Warning, TEXT("Construction: Update Origin"));

}


FVector AConstruction::GetIntersectLoc() const{
	return intersectLoc;
}

