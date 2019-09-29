// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RuntimeDrawingLibrary.generated.h"

/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API URuntimeDrawingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static void FlushPersistentLines(const UWorld* InWorld);
	static void DrawBox(const UWorld* InWorld, FVector const& Center, FVector const& Box, FColor const& Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority, float Thickness);
	
};
