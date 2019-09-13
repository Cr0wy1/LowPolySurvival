// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WidgetAsset.generated.h"



class UDebugWidget;
class UEscapeMenuWidget;
class UChatWidget;
class UMapWidget;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UWidgetAsset : public UDataAsset
{
	GENERATED_BODY()
	

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UDebugWidget> debugWidget_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UEscapeMenuWidget> escapeMenuWidget_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UChatWidget> chatWidget_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UMapWidget> mapWidget_BP;
};
 