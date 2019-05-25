// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIWidget.h"
#include "PlacementMenuWidget.generated.h"



class UCheckBox;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UPlacementMenuWidget : public UUIWidget
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION()
	FEventReply OnKeyUp(FGeometry MyGeometry, FKeyEvent InKeyEvent);

	UPROPERTY(meta = (BindWidget))
	UCheckBox* gridCheck;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* objectCheck;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* resetRotCheck;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* intersectCheck;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* lockRotCheck;

	void ToggleGridSnap();
	void ToggleObjectSnap();
	void ToggleIntersect();


	bool IsGridSnapChecked() const;
	bool IsObjectSnapChecked() const;
	bool IsResetRotChecked() const;
	bool IsIntersectChecked() const;
	bool IsLockRotChecked() const;
};
