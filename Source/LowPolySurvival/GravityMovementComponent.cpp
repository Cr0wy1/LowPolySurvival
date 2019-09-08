// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h" //Find At Rotation
#include "GravityComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGravityMovementComponent::UGravityMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGravityMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGravityMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//UE_LOG(LogTemp, Warning, TEXT("UGravityMovementComponent: I am ticking!"));
	if (bIsInGravitySphere && primitiveComp) {
		//AActor* owner = GetOwner();

		FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(gravityCenter, primitiveComp->GetComponentLocation());
		FVector lookForwardVec = UKismetMathLibrary::GetForwardVector(lookAtRotation);

		FRotator zxRot = UKismetMathLibrary::MakeRotFromZX(lookForwardVec, primitiveComp->GetForwardVector());

		
		GetOwner()->SetActorRotation(zxRot);

		//ACharacter* pawn;
		//primitiveComp->AddForce(FVector(0, 0, 1) * 100);
		primitiveComp->SetPhysicsLinearVelocity(lookForwardVec*-10, true);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *(lookForwardVec*-10).ToString());

		
	}

}

void UGravityMovementComponent::SetAffectedComp(UPrimitiveComponent * _primitiveComp){
	primitiveComp = _primitiveComp;
	
}

void UGravityMovementComponent::SetGravity(FVector _gravityCenter){
	gravityCenter = _gravityCenter;
	bIsInGravitySphere = true;
}

void UGravityMovementComponent::RemoveGravity(){
	bIsInGravitySphere = false;
}

