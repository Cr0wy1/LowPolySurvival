#pragma once


#include "CoreMinimal.h"
#include "GameStructs.generated.h"



UENUM(BlueprintType)
enum class EBiome : uint8 {
	GRASS,
	FOREST,
	SAND,
	HILLS,

};



USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FBlockData {
	GENERATED_BODY()

		int32 blockId = 0;

	float value = 0;

	FColor color = FColor::Green;

	FBlockData(int32 _blockId = 0) : blockId(_blockId) {}
};