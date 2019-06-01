// Fill out your copyright notice in the Description page of Project Settings.

#include "PlaceWidget.h"
#include "Components/WidgetComponent.h"
#include "PlacementWidget.h"
#include "Components/ArrowComponent.h"


// Sets default values
APlaceWidget::APlaceWidget(){

	PrimaryActorTick.bCanEverTick = false;

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

UWidgetComponent * APlaceWidget::GetHoveredWidgetComp() const {

	for (FWidgetArrows widgetArrows : widgetArrowsStructs) {
		//if (widgetArrows.widgetComp->GetUserWidgetObject()->IsHovered()) {
			//return widgetArrows.widgetComp;
		//}
	}

	return nullptr;
}

int32 APlaceWidget::GetHoveredWidgetIndex() const {

	for (size_t i = 0; i < widgetArrowsStructs.Num(); i++) {
		if (widgetArrowsStructs[i].widgetComp->GetUserWidgetObject()->IsHovered()) {
			return i;
		}
	}

	return -1;
}

FTransform APlaceWidget::GetPlacementTransform() const{

	FTransform returnTransform;

	int32 iHoveredWidget = GetHoveredWidgetIndex();

	if (iHoveredWidget > -1) {

		UPlacementWidget* placementWidget = Cast<UPlacementWidget>(widgetArrowsStructs[iHoveredWidget].widgetComp->GetUserWidgetObject());

		if (placementWidget) {
			//UE_LOG(LogTemp, Warning, TEXT("%i"), placementWidget->hoveredIndex);

			if (placementWidget->hoveredIndex < widgetArrowsStructs[iHoveredWidget].arrowComps.Num()) {

				returnTransform = widgetArrowsStructs[iHoveredWidget].arrowComps[placementWidget->hoveredIndex]->GetComponentTransform();

			}

		}
	}


	return returnTransform;
}


void APlaceWidget::UpdateAllWidgetComps() {

	TArray<UActorComponent*> actorComps = GetComponentsByClass(UWidgetComponent::StaticClass());

	for (UActorComponent* actorComp : actorComps) {

		FWidgetArrows newWidgetArrow;

		newWidgetArrow.widgetComp = Cast<UWidgetComponent>(actorComp);

		TArray<USceneComponent*> widgetChildComps = newWidgetArrow.widgetComp->GetAttachChildren();

		for (USceneComponent* widgetChild : widgetChildComps) {

			UArrowComponent* arrowComp = Cast<UArrowComponent>(widgetChild);

			if (arrowComp) {
				newWidgetArrow.arrowComps.Push(arrowComp);
			}

		}

		widgetArrowsStructs.Push(newWidgetArrow);

	}

}

