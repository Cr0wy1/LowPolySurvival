// Fill out your copyright notice in the Description page of Project Settings.

#include "PlacementMenuWidget.h"
#include "CheckBox.h"

FEventReply UPlacementMenuWidget::OnKeyUp(FGeometry MyGeometry, FKeyEvent InKeyEvent) {

	if (InKeyEvent.GetKey() == EKeys::V) {
		CloseUI();
	}
	

	UE_LOG(LogTemp, Warning, TEXT("PLacemntManuKeyUp"));

	return FEventReply(true);
}

void UPlacementMenuWidget::ToggleGridSnap(){
	gridCheck->SetIsChecked(!gridCheck->IsChecked());
}

void UPlacementMenuWidget::ToggleObjectSnap(){
	objectCheck->SetIsChecked(!objectCheck->IsChecked());
}

void UPlacementMenuWidget::ToggleIntersect(){
	intersectCheck->SetIsChecked(!intersectCheck->IsChecked());
}

bool UPlacementMenuWidget::IsGridSnapChecked() const{

	return gridCheck->IsChecked();
}

bool UPlacementMenuWidget::IsObjectSnapChecked() const
{
	return objectCheck->IsChecked();
}

bool UPlacementMenuWidget::IsResetRotChecked() const
{
	return resetRotCheck->IsChecked();
}

bool UPlacementMenuWidget::IsIntersectChecked() const
{
	return intersectCheck->IsChecked();
}
