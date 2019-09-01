#pragma once


#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameStructs.generated.h"



class UMyGameInstance;



using FBlockLoc = FIntVector;
using FChunkLoc = FIntVector;

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


UENUM(BlueprintType)
enum class EResourceType : uint8 {
	SOLID,
	FLUID,
	GAS,
};


UENUM(BlueprintType)
enum class EPlayerMode : uint8 {
	SURVIVAL,
	CREATIVE,
	SPECTATOR,
};


UENUM(BlueprintType)
enum class EGridDir : uint8 {
	FRONT,
	RIGHT,
	UP,
	BACK,
	LEFT,
	DOWN,
};


USTRUCT()
struct LOWPOLYSURVIVAL_API FGridDir {

	GENERATED_BODY()

	static const FIntVector FRONT;
	static const FIntVector RIGHT;
	static const FIntVector UP;
	static const FIntVector BACK;
	static const FIntVector LEFT;
	static const FIntVector DOWN;
};

struct FBlock;


 
USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FBlockData {
	GENERATED_BODY()

	//int32 blockId = 0;
	bool bIsSolid = false;
	FColor color = FColor::Green;

	//FBlockData(int32 _blockId = 0) : blockId(_blockId) {}

	
	
	FString ToString() const;
};

USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FBlock {
	GENERATED_BODY()

	FBlockData data;

	int32 durability;

	const FResource* resource = nullptr;

	//Return true if durability <= 0 (Block is destroyed)
	bool ApplyDamage(int32 damageAmount);
	void SetResource(const FResource* _resource);
	void SetBiomeColor(const FLinearColor& biomeColor);

	bool IsValid() const;
	bool IsAir() const;

	static FBlock FromId(AActor* owner, int32 resourceId);

	FString ToString() const;
};


USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FBlockGrid {
	GENERATED_BODY()

	FIntVector dims;
	TArray<TArray<TArray<FBlock>>> grid;

	FBlockGrid();
	FBlockGrid(int32 xSize, int32 ySize, int32 zSize);

	void Init(int32 xSize, int32 ySize, int32 zSize);

	bool IsValidIndex(int32 xIndex) const;
	bool IsValidIndex(int32 x, int32 y, int32 z) const;
	bool IsValidIndex(const FIntVector &Index3D) const;

	//Checks if index is Valid and return a block pointer, if is not valid return nullptr
	const FBlock* GetBlockSafe(const FIntVector &Index3D) const;

	TArray<TArray<FBlock>>& operator[](int32 xIndex);
	const TArray<TArray<FBlock>>& operator[](int32 xIndex) const;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResourceType type = EResourceType::SOLID;

	static FResource* FromId(AActor* owner, int32 resourceId);

	FString ToString() const;
};


USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FBiomeData : public FTableRowBase {
	GENERATED_BODY()

	struct DATA {
		static FBiomeData* GRAS;
		static FBiomeData* DESERT;
		static FBiomeData* TUNDRA;
		static FBiomeData* GRASDESERT;
		static FBiomeData* SAVANNA;
		static FBiomeData* WOODS;
		static FBiomeData* TAIGA;
		static FBiomeData* SEASONALFOREST;
		static FBiomeData* FOREST;
		static FBiomeData* RAINFOREST;
		static FBiomeData* SWAMP;
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 id = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName baseBlockResourceId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float noiseScaleXY = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float heightScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool overrideResourceColor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor biomeColor = FLinearColor(1,1,1,1);

	FResource* baseBlockResource = nullptr;

	static void InitStaticMembers(UMyGameInstance* gameInstance);
};


