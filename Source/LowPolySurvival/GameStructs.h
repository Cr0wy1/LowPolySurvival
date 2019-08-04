#pragma once


#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameStructs.generated.h"


UENUM(BlueprintType)
enum class EVegetationType : uint8 {
	NONE,
	GRASS,
	TREE,
	STONES,
};



UENUM(BlueprintType)
enum class EBiome : uint8 {
	GRASS,
	FOREST,
	SAND,
	HILLS,

};


struct FBlock;
 
USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FBlockData {
	GENERATED_BODY()

	//int32 blockId = 0;
	bool bIsSolid = false;
	FColor color = FColor::Green;

	//FBlockData(int32 _blockId = 0) : blockId(_blockId) {}
	
};

USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FBlock {
	GENERATED_BODY()

	FBlockData data;

	int32 durability;

	FResource* resource = nullptr;

	FBlock(int32 blockId = 0) {
		data.blockId = blockId;
	}

	//Return true if durability <= 0 (Block is destroyed)
	bool ApplyDamage(int32 damageAmount);
	void SetResource(FResource* _resource);
	bool IsValid() const;
};



USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FGenerationParams {
	GENERATED_BODY()



};

//DataTable Structs
USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FWorldGenInfo : public FTableRowBase {
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 id = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AActor> Plant_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EBiome biome = EBiome::GRASS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EVegetationType type = EVegetationType::NONE;
};


USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FResource : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 id = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName name = "NONE";

	//minimum Level for damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 solidity = 1;

	//How many durability a Recource have
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 hardness = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor color = FLinearColor(100, 0, 100, 0);

	static FResource* FromId(AActor* owner, int32 resourceId);
};

