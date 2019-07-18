// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chunk.generated.h"



class UMyGameInstance;
class UProceduralMeshGeneratorComponent;
class AWorldGenerator;
class AChunkColumn;
struct FWorldInfo;
struct FBlockData;


UCLASS()
class LOWPOLYSURVIVAL_API AChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunk();

	void Init(AWorldGenerator* _worldGenerator, AChunkColumn* _chunkColumn);

protected:

	bool bIsTerrainGenerated = false;

	UMyGameInstance * gameInstance = nullptr;
	AWorldGenerator* worldGenerator = nullptr;
	AChunkColumn* chunkColumn = nullptr;

	FIntVector gridDim;
	TArray<TArray<TArray<FBlockData>>> blockGrid;

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

	UPROPERTY(VisibleAnywhere)
	FIntVector chunkLoc;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Create(FIntVector _chunkLoc);
	void Load(FIntVector _chunkLoc);
	void Unload();

	void GenerateTerrainMesh();
	void UpdateTerrainMesh(const FIntVector &blockLocation);

	void RemoveBlock(int32 gridX, int32 gridY, int32 gridZ);
	void RemoveBlock(FIntVector gridLoc);

	void SetTerrainMaterial(UMaterialInterface* material);

	const TArray<TArray<TArray<FBlockData>>>* GetGridData() const;
	const AWorldGenerator* GetWorldGenerator() const;
	FIntVector GetChunkLocation() const;

	TArray<TArray<FBlockData>>& operator[](int32 index);
	const TArray<TArray<FBlockData>>& operator[](int32 index) const;
};
