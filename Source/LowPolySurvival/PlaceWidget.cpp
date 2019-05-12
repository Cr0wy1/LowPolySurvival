// Fill out your copyright notice in the Description page of Project Settings.

#include "PlaceWidget.h"
#include "Components/WidgetComponent.h"


// Sets default values
APlaceWidget::APlaceWidget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlaceWidget::BeginPlay()
{
	Super::BeginPlay();

	UpdateAllWidgetComps();

}

// Called every frame
void APlaceWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UWidgetComponent * APlaceWidget::GetHoveredWidgetComp() const{

	for (UWidgetComponent* widgetComp : widgetComps) {
		if (widgetComp->GetUserWidgetObject()->IsHovered()) {
			return widgetComp;
		}
	}

	return nullptr;
}


void APlaceWidget::UpdateAllWidgetComps() {

	TArray<UActorComponent*> actorComps = GetComponentsByClass(UWidgetComponent::StaticClass());

	for (UActorComponent* actorComp : actorComps) {
		UWidgetComponent* widgetComp = Cast<UWidgetComponent>(actorComp);
		widgetComps.Push(widgetComp);
	}

	UE_LOG(LogTemp, Warning, TEXT("%i"), widgetComps.Num());


}

