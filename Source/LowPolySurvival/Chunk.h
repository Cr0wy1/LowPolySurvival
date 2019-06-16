// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chunk.generated.h"



class UMyGameInstance;



UCLASS()
class LOWPOLYSURVIVAL_API AChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunk();

protected:

	UMyGameInstance * gameInstance = nullptr;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void TopDownTrace();
public:	

	FVector2D chunkLoc;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Create(FVector2D _chunkLoc);
	void Load(FVector2D _chunkLoc);
	void Unload();
};
