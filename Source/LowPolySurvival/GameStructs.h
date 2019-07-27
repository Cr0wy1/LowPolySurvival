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

	int32 blockId = 0;

	FColor color = FColor::Green;

	FBlockData(int32 _blockId = 0) : blockId(_blockId) {}
	
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
};


//###### Texture Generation Code ######
//int32 w, h;
//w = slotSelectedIcon->GetSizeX();
//h = slotSelectedIcon->GetSizeY();
//int32 mDataSize = w * h * 4;
//int32 mDataSqrtSize = w * 4;
//
//uint8* mTextureColors = new uint8[mDataSize];
//uint8* mDynamicColors = new uint8[mDataSize];
//
//FUpdateTextureRegion2D * mUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, w, h);
//
//UTexture2D* dynamicTexture = UTexture2D::CreateTransient(w, h);
//dynamicTexture->UpdateResource();
//
//FTexture2DMipMap& readMap = slotSelectedIcon->PlatformData->Mips[0];
//readMap.BulkData.GetCopy((void**)&mTextureColors);
//
//FMemory::Memcpy(mDynamicColors, mTextureColors, mDataSize);
//
//
//
//int32 pixelAmount = mDataSize / 4;
//for (int32 i = 0; i < pixelAmount; ++i)
//{
//	int32 blue = i * 4 + 0;
//	int32 green = i * 4 + 1;
//	int32 red = i * 4 + 2;
//	int32 alpha = i * 4 + 3;
//	mDynamicColors[blue] = 0;
//	mDynamicColors[green] = 0;
//	mDynamicColors[red] = FMath::Rand(); // Set pixel's red value to 120
//	mDynamicColors[alpha] = 255;
//}
//
//dynamicTexture->UpdateTextureRegions(0, 1, mUpdateTextureRegion, mDataSqrtSize, (uint32)4, mDynamicColors);
//
//delete[] mTextureColors;
//delete[] mDynamicColors;