// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameStructs.h"
#include "HAL/Runnable.h"
#include "Chunk.generated.h"



class UMyGameInstance;
class UProceduralMeshGeneratorComponent;
class AWorldGenerator;
class UChunkColumn;
class UGridNoiseGen;
struct FWorldInfo;
struct FBlockData;
struct FBlock;



UENUM()
enum class EChunkGenStage : uint8 {
	INIT,
	NOISEGEN,
	MESHGEN,
	DECOPLACE,
	FINISH,
};




UCLASS()
class LOWPOLYSURVIVAL_API AChunk : public AActor
{
	GENERATED_BODY()

	friend class ChunkTask;

private:
	//Development
	bool bDrawDebug = false;

	EChunkGenStage chunkGenStage = EChunkGenStage::INIT;

public:	

	bool bNeedsFluidUpdate = false;
	// Sets default values for this actor's properties
	AChunk();

	void Init(AWorldGenerator* _worldGenerator, UChunkColumn* _chunkColumn);


protected:

	bool bIsAirChunk = false;

	bool bIsTaskFinished = false;
	bool bShouldGenerateMesh = false;
	
	bool bNeedsSaving = false;

	bool bIsFullCreated = false;
	bool bIsTerrainGenerated = false;

	UGridNoiseGen* gridNoiseGen;

	UMyGameInstance * gameInstance = nullptr;
	AWorldGenerator* worldGenerator = nullptr;

	//UPROPERTY()
	UChunkColumn* chunkColumn;

	FIntVector gridDim;
	FBlockGrid blockGrid;

	TArray<AActor*> loadedActors;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	UProceduralMeshGeneratorComponent* proceduralMesh;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Create();

	void InitBlockGrid();
	void GenerateNoise();
	

	void TopDownTrace(const FIntVector &blockLoc);



	void RandomizeGrid(int32 zLine, int32 blockAmount);
	void ApplyNoiseOnGrid();
	void ApplyNoiseOnGrid3D();
	void AddNoiseCaves();
	void AddNoiseOres();
	void AddWater();

	UFUNCTION()
	void OnGeneratedMesh();

	void FluidUpdate();

	//Save Load
	void SaveToFile();
	bool LoadFromFile();

	

public:	

	UPROPERTY(VisibleAnywhere)
	FIntVector chunkLoc;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static AChunk* Construct(AWorldGenerator* _worldGenerator, FIntVector _chunkLoc);
	
	void Load(FIntVector _chunkLoc);
	void Unload();

	UFUNCTION()
	void GenerateTerrainMesh();

	void UpdateTerrainMesh();
	void UpdateTerrainMesh(const FIntVector &chunkBlockLoc);

	void DrawDebug(bool bEditorOnly = true, bool bWithBlocks = false);

	//returns true if Block gets destroyed
	bool HitBlock(FIntVector gridLoc, float damageAmount, AActor* causer);
	bool SetBlock(FIntVector gridLoc, const FBlock& block);
	void SetBlockUnsafe(FIntVector gridLoc, const FBlock& block);

	void SetTerrainMaterial(UMaterialInterface* material);

	const FBlock* GetBlock(const FIntVector &chunkBlockLoc) const;
	const FBlock* GetBlock(const FVector &worldLoc) const;
	FBlock* GetBlockIncludeNearby(FIntVector chunkBlockLoc, AChunk* OUT_targetChunk = nullptr) const;

	bool IsAirChunk() const;

	const FBlockGrid* GetGridData() const;
	AWorldGenerator* GetWorldGenerator() const;
	FIntVector GetChunkLocation() const;

	//Return nearby Chunk if exits, else return nullptr
	AChunk* GetNearbyChunk(EGridDir gridDir) const;
	
	//Return chunk Array with nearby chunks, if gridDirs is empty, return all nearby
	TArray<AChunk*> GetNearbyChunks(TArray<EGridDir> gridDirs = TArray<EGridDir>()) const;

	TArray<TArray<FBlock>>& operator[](int32 index);
	const TArray<TArray<FBlock>>& operator[](int32 index) const;

	FBlock& operator[](const FIntVector &chunkBlockLoc);
	const FBlock& operator[](const FIntVector &chunkBlockLoc) const;

	static void OnEndTask(AChunk* chunk);
};


class ChunkTask : public FNonAbandonableTask {

	AChunk * chunk;

	//true = save, false = load
	bool bSaveInsteadOfLoad;

public:
	ChunkTask(AChunk * _chunk, bool _bSaveInsteadOfLoad);
	~ChunkTask();

	void DoWork();


	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(ExampleAutoDeleteAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

};