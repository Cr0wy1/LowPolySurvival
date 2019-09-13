// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameStructs.h"
#include "ChunkLoader.generated.h"



class AWorldGenerator;
/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LOWPOLYSURVIVAL_API UChunkLoaderComponent : public UActorComponent
{
	GENERATED_BODY()
		 
public:

	UChunkLoaderComponent(const FObjectInitializer & ObjectInitializer);

	void UpdateLocation(FChunkLoc newCenterChunkLoc);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	AActor * owner = nullptr;

	//current ChunkLoader location in chunkLoc
	FChunkLoc cChunkLoc = FChunkLoc(99999,99999,99999);

	AWorldGenerator * worldGenerator;

	TArray<FIntVector> checkedChunkLocs;

	//Dynamic = chunkLoader is ticking and Update position automaticlly
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChunkLoader")
	bool bIsDynamic = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChunkLoader")
	int32 checkedRadiusZ = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChunkLoader")
	int32 checkedRadiusXY = 2;

	void BeginPlay();

	void OnCheckChunk(FIntVector chunkLoc);
	void LoadChunk(FIntVector chunkLoc);
	void CheckChunks(FIntVector center);
};
