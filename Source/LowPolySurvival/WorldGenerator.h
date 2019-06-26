// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "WorldGenerator.generated.h"


class APlayercharController;
class AChunk;


UENUM(BlueprintType)
enum class EBiome : uint8 {
	GRASS,
	FOREST,
	SAND,
	HILLS,

};


USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FWorldGenInfo : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 id = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Plant_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBiome biome = EBiome::GRASS;
};



UCLASS()
class LOWPOLYSURVIVAL_API AWorldGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldGenerator();

protected:

	bool bDrawDebug = false;

	float chunkSize;
	int32 checkedRadius = 2;
	 
	APlayercharController * playerController = nullptr;

	FVector2D cPlayerChunkLoc;

	TArray<FVector2D> createdChunks;
	TMap<FVector2D, AChunk*> loadedChunks;
	TArray<FVector2D> checkedChunkLocs;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnEnterChunk(); 
	void OnCheckChunk(FVector2D chunkLoc);

	void LoadChunk(FVector2D chunkLoc);
	

public:	
	// Called every frame  
	virtual void Tick(float DeltaTime) override; 

	void CheckChunks(int32 centerX, int32 centerY) ;

};
