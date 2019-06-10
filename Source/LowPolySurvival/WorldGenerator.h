// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldGenerator.generated.h"


class APlayercharController;
class AChunk;


UCLASS()
class LOWPOLYSURVIVAL_API AWorldGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldGenerator();

protected:

	int32 chunkSize = 10000;
	int32 checkedRadius = 15;

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
