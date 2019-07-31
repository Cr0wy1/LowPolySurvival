// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunctionLibrary.generated.h"



class UTexture2D;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

public:

	UFUNCTION(BlueprintCallable, Category = "Procedural")
	static UTexture2D* CreateTextureFromTextures(const TArray<UTexture2D*> textures);

	UFUNCTION(BlueprintCallable, Category = "Procedural")
	static UTexture2D* CreateColoredTexture(UTexture2D* texture, const FLinearColor &color);
};
