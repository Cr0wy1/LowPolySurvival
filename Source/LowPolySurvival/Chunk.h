// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chunk.generated.h"



class UMyGameInstance;
class UProceduralMeshGeneratorComponent;
class AWorldGenerator;
class UChunkColumn;
struct FWorldInfo;
struct FBlockData;
struct FBlock;




UCLASS()
class LOWPOLYSURVIVAL_API AChunk : public AActor
{
	GENERATED_BODY()

private:
	//Development
	bool bDrawDebug = false;

public:	
	// Sets default values for this actor's properties
	AChunk();

	void Init(AWorldGenerator* _worldGenerator, UChunkColumn* _chunkColumn);

protected:

	bool bIsFullCreated = false;
	bool bIsTerrainGenerated = false;

	UMyGameInstance * gameInstance = nullptr;
	AWorldGenerator* worldGenerator = nullptr;
	UChunkColumn* chunkColumn = nullptr;

	FIntVector gridDim;
	TArray<TArray<TArray<FBlock>>> blockGrid;

	TArray<AActor*> loadedActors;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	UProceduralMeshGeneratorComponent* proceduralMesh;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

	void InitBlockGrid();

	void TopDownTrace(const FIntVector &blockLoc);



	void RandomizeGrid(int32 zLine, int32 blockAmount);
	void ApplyNoiseOnGrid();
	void AddNoiseCaves();
	void AddNoiseOres();

	UFUNCTION()
	void OnGeneratedMesh();

	//Save Load
	void SaveToFile();
	bool LoadFromFile();

public:	

	UPROPERTY(VisibleAnywhere)
	FIntVector chunkLoc;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Create(FIntVector _chunkLoc);
	void Load(FIntVector _chunkLoc);
	void Unload();

	void GenerateTerrainMesh();
	void UpdateTerrainMesh(const FIntVector &chunkBlockLoc);

	void HitBlock(FIntVector gridLoc, float damageAmount, AActor* causer);
	void SetBlock(FIntVector gridLoc, const FBlockData &blockData);
	void SetBlockUnsafe(FIntVector gridLoc, const FBlockData &blockData);

	void SetTerrainMaterial(UMaterialInterface* material);

	const FBlock& GetBlock(const FIntVector &chunkBlockLoc) const;
	const FBlock& GetBlock(const FVector &worldLoc) const;
	const TArray<TArray<TArray<FBlock>>>* GetGridData() const;
	const AWorldGenerator* GetWorldGenerator() const;
	FIntVector GetChunkLocation() const;

	TArray<TArray<FBlock>>& operator[](int32 index);
	const TArray<TArray<FBlock>>& operator[](int32 index) const;

	FBlock& operator[](const FIntVector &chunkBlockLoc);
	const FBlock& operator[](const FIntVector &chunkBlockLoc) const;
};
