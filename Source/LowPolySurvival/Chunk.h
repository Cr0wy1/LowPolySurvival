// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chunk.generated.h"



class UMyGameInstance;
class UProceduralMeshGeneratorComponent;
struct FWorldInfo;

USTRUCT()
struct LOWPOLYSURVIVAL_API FBlockInfo {
	GENERATED_BODY()
		 
	//FBlockInfo(float _value) : value(_value) {}

	float value = 0;
};


UCLASS()
class LOWPOLYSURVIVAL_API AChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunk();

protected:

	

	UMyGameInstance * gameInstance = nullptr;
	const FWorldInfo* worldInfo = nullptr;

	TArray<TArray<TArray<FBlockInfo>>> blockGrid;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	UProceduralMeshGeneratorComponent* proceduralMesh;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitBlockGrid();

	void TopDownTrace();

	//Development
	bool bDrawDebug = true;

	void RandomizeGrid(int32 zLine, int32 blockAmount);
	void ApplyNoiseOnGrid();

public:	

	FVector2D chunkLoc;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Create(FVector2D _chunkLoc);
	void Load(FVector2D _chunkLoc);
	void Unload();
};
