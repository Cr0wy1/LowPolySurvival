// Fill out your copyright notice in the Description page of Project Settings.

#include "PlaceWidget.h"
#include "WidgetPlaceComponent.h"
#include "PlacementWidget.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshSocket.h"


// Sets default values
APlaceWidget::APlaceWidget(){

	meshTemplate = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Template");
	meshTemplate->SetupAttachment(RootComponent);
	meshTemplate->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	meshTemplate->SetVisibility(false);

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


int32 APlaceWidget::GetHoveredWidgetIndex() const {

	for (size_t i = 0; i < snappingSurfaces.Num(); i++) {
		if (snappingSurfaces[i].widgetPlaceComp->GetUserWidgetObject()->IsHovered()) {
			return i;
		}
	}

	return -1;
}



FTransform APlaceWidget::GetPlacementTransform(int32 &margin) const{

	FTransform returnTransform;

	int32 iHoveredWidget = GetHoveredWidgetIndex();

	if (snappingSurfaces.IsValidIndex(iHoveredWidget)) {

		UPlacementWidget* placementWidget = Cast<UPlacementWidget>(snappingSurfaces[iHoveredWidget].widgetPlaceComp->GetUserWidgetObject());

		if (placementWidget) {

			if (snappingSurfaces[iHoveredWidget].socketNames.IsValidIndex(placementWidget->hoveredIndex)) {
				FTransform arrowTrans = meshTemplate->GetSocketTransform(snappingSurfaces[iHoveredWidget].socketNames[placementWidget->hoveredIndex]);
				returnTransform = arrowTrans;
				margin = snappingSurfaces[iHoveredWidget].margins[placementWidget->hoveredIndex];
			}

		}
	}


	return returnTransform;
}


void APlaceWidget::UpdateAllWidgetComps() {

	TArray<UActorComponent*> actorComps = GetComponentsByClass(UWidgetPlaceComponent::StaticClass());

	for (UActorComponent* actorComp : actorComps) {

		UWidgetPlaceComponent* widgetPlaceComp = Cast<UWidgetPlaceComponent>(actorComp);


		if (widgetPlaceComp) {
			FSnappingSurface newSurface;
			newSurface.widgetPlaceComp = widgetPlaceComp;
			snappingSurfaces.Push(newSurface);
		}

	}

	TArray<FName> socketNames = meshTemplate->GetAllSocketNames();

	int32 iSurface = 0;


	for (size_t i = 0; i < socketNames.Num(); i++){
		
		const UStaticMeshSocket* socket = meshTemplate->GetSocketByName(socketNames[i]);

		FString index, margin;

		int32 iChar;
		if (socket->Tag.FindChar(',', iChar)) {
			
			socket->Tag.Split(",", &index, &margin);
		}
		else {
			index = socket->Tag;
			margin = "0";
		}

		UE_LOG(LogTemp, Warning, TEXT("Socket Tag: %s %s"), *index, *margin);
		

		if (FString::FromInt(iSurface) != index) {
			++iSurface;
		}

		if (!snappingSurfaces.IsValidIndex(iSurface)) {
			break;

		}



		snappingSurfaces[iSurface].socketNames.Push(socketNames[i]);
		snappingSurfaces[iSurface].margins.Add(FCString::Atoi(*margin));
		
	}
}

