// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityComponent.h"
#include "GravityMovementComponent.h"

// Sets default values for this component's properties
UGravityComponent::UGravityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	OnComponentBeginOverlap.AddDynamic(this, &UGravityComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UGravityComponent::OnEndOverlap);
}


// Called when the game starts
void UGravityComponent::BeginPlay()
{
	Super::BeginPlay();
	 
	
}

void UGravityComponent::OnBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult){
	
	UGravityMovementComponent* gravityMoveComp =  Cast<UGravityMovementComponent>(OtherActor->GetComponentByClass(UGravityMovementComponent::StaticClass()));
	if (gravityMoveComp) {
		gravityMoveComp->SetGravity(GetComponentLocation());
		UE_LOG(LogTemp, Warning, TEXT("OnBeginOverlap!"));
	} 

}

void UGravityComponent::OnEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex){
	UGravityMovementComponent* gravityMoveComp = Cast<UGravityMovementComponent>(OtherActor->GetComponentByClass(UGravityMovementComponent::StaticClass()));
	if (gravityMoveComp) {
		//gravityMoveComp->RemoveGravity();
		UE_LOG(LogTemp, Warning, TEXT("OnEndOverlap!"));
	}
}


// Called every frame
void UGravityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//UE_LOG(LogTemp, Warning, TEXT(" UGravityComponent: I am ticking!"));

}

