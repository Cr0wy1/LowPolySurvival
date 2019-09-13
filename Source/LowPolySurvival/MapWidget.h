// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIWidget.h"
#include "MapWidget.generated.h"


class UImage;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UMapWidget : public UUIWidget
{
	GENERATED_BODY()
	

public:
	void RenderMap();

protected:

	UPROPERTY(meta = (BindWidget))
		UImage* mapImage;
};
