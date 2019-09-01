// Fill out your copyright notice in the Description page of Project Settings.


#include "CrosshairTraceComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Chunk.h" //remove this, just for testing

// Sets default values for this component's properties
UCrosshairTraceComponent::UCrosshairTraceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
}


// Called when the game starts
void UCrosshairTraceComponent::BeginPlay(){
	Super::BeginPlay();

	playerController = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	
	if (bIgnoreOwner) {
		traceParams.AddIgnoredActor(GetOwner());
	}
	
}


// Called every frame
void UCrosshairTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CrosshairLineTrace(crosshairResult.hitResult, crosshairResult.hitDirection);
	
}


bool UCrosshairTraceComponent::CrosshairLineTrace(FHitResult &OUT_hitresult, FVector &OUT_Direction) {
	
	FVector startLocation, endLocation;
	int32 viewX, viewY;
	
	playerController->GetViewportSize(viewX, viewY);
	playerController->DeprojectScreenPositionToWorld(viewX * 0.5f, viewY * 0.5f, startLocation, OUT_Direction);

	endLocation = OUT_Direction * traceLength + startLocation;

	GetWorld()->LineTraceSingleByChannel(OUT_hitresult, startLocation, endLocation, ECollisionChannel::ECC_GameTraceChannel2, traceParams);

	if (OUT_hitresult.GetActor()) {
		//DrawDebugLine(GetWorld(), startLocation, hitResult.ImpactPoint, FColor::Red, false, -1.0f, 0, 1.0f);

		//FVector blockCenter = FVector(WorldToBlockLocation(OUT_hitresult.ImpactPoint + (OUT_Direction * 25))) * 100;
		//DrawDebugBox(GetWorld(), blockCenter, FVector(50, 50, 50), FColor::Purple, false, -1, 0, 3);
	}

	DrawDebugSphere(GetWorld(), OUT_hitresult.ImpactPoint, 50, 10, FColor::Red); 


	return true;
}

