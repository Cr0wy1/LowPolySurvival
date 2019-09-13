// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameStructs.h"
#include "BiomeGenLibrary.generated.h"



class UMyGameInstance;


USTRUCT()
struct LOWPOLYSURVIVAL_API FSBiomeDiagram {
	GENERATED_BODY()
	
	static FBiomeDiagram firstBiomeDiagram;
	static FBiomeDiagram baseBiomeDiagram;

	static void InitStaticMembers();
};



USTRUCT()
struct LOWPOLYSURVIVAL_API FBiomeDiagram {
	GENERATED_BODY()

	

	int32 xNum;
	int32 yNum;

	TArray<TArray<FBiomeData*>> diagramData;

	FBiomeDiagram(){}

	FBiomeDiagram(std::initializer_list<TArray<FBiomeData*>> initList) : diagramData(initList){
		yNum = diagramData.Num();
		xNum = diagramData[0].Num();
	}

	FBiomeData* GetBiome(float xPercent, float yPercent) const;
	FBiomeData* GetBiome(float xPercent, float yPercent, bool xInvert, bool yInvert) const;
};
/**
 * 
 */
UCLASS() 
class LOWPOLYSURVIVAL_API UBiomeGenLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	

public:
};
