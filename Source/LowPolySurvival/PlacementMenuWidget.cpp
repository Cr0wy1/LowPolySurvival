// Fill out your copyright notice in the Description page of Project Settings.

#include "PlacementMenuWidget.h"
#include "CheckBox.h"


FReply UPlacementMenuWidget::NativeOnKeyUp(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent){
	if (InKeyEvent.GetKey() == EKeys::V) {
		CloseUI();
	}

	return FReply(Super::NativeOnKeyUp(InGeometry, InKeyEvent));
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

bool UPlacementMenuWidget::IsLockRotChecked() const{

	return lockRotCheck->IsChecked();

}
