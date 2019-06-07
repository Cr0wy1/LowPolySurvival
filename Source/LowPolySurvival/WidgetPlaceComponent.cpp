// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetPlaceComponent.h"

// Sets default values for this component's properties
UWidgetPlaceComponent::UWidgetPlaceComponent(const FObjectInitializer & ObjectInitializer) : UWidgetComponent(ObjectInitializer){

	PrimaryComponentTick.bCanEverTick = true;

	snappingPoints.Add(FTransform(FRotator(90, 0, 0), FVector(-10, 0, 40)));
}


// Called when the game starts
void UWidgetPlaceComponent::BeginPlay()
{
	Super::BeginPlay();

	//PrimaryComponentTick.SetTickFunctionEnable(false);



}


// Called every frame
void UWidgetPlaceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//UE_LOG(LogTemp, Warning, TEXT("AttributComp ticking"));

}

FTransform UWidgetPlaceComponent::GetSnappingPoint(int32 index) const{

	FTransform returnTrans = GetComponentTransform();
	
	if (snappingPoints.IsValidIndex(index)) {
		returnTrans.SetLocation(FTransform::AddTranslations(returnTrans, snappingPoints[index]));
		//returnTrans.SetRotation(snappingPoints[index].GetRotation());
		
	}

	return returnTrans;
}
