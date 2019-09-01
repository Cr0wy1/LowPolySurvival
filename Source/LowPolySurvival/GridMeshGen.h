// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameStructs.h"
#include "MeshGenerationLibrary.h" // FOccluderVertexArray and FProcMeshTangent
#include "HAL/Runnable.h"
#include "GridMeshGen.generated.h"



class AChunk;



USTRUCT()
struct LOWPOLYSURVIVAL_API FPositiveConnectedBlockGrids {
	GENERATED_BODY()

	FBlockGrid* FRONT;
	FBlockGrid* RIGHT;
	FBlockGrid* UP;
	FBlockGrid* FRONTRIGHT;
	FBlockGrid* FRONTUP;
	FBlockGrid* RIGHTUP;
	FBlockGrid* FRONTRIGHTUP;
};


USTRUCT()
struct LOWPOLYSURVIVAL_API FGridMeshParams {
	GENERATED_BODY()

	FPositiveConnectedBlockGrids posConnBlockGrids;
	const AChunk* chunk = nullptr;

};


/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UGridMeshGen : public UObject
{
	GENERATED_BODY()

	friend class GridMeshGenTask;
	friend class GridMeshGenThread;

protected:

	FMeshGenData meshGenData;
	FIntVector gridSize;
	FGridMeshParams params;
	 
	FAsyncTask<GridMeshGenTask>* task;
	GridMeshGenThread* gridMeshGenThread;

public:
	virtual void EnsureCompletion();

	virtual FMeshGenData* CreateMesh(const FBlockGrid &blockGrid, const FGridMeshParams &_params = FGridMeshParams());
	virtual FMeshGenData* UpdateMesh(const FBlockGrid &blockGrid, const FIntVector &blockLoc, const FGridMeshParams &_params = FGridMeshParams());

	bool IsReady() const;

protected:

	virtual void DoTaskWork(bool bUpdateOnly, const FBlockGrid &blockGrid);

};


class GridMeshGenTask : public FNonAbandonableTask {


	UGridMeshGen* gridMeshGen;
	const FBlockGrid* blockGrid;
	bool bUpdateOnly = false;

public:
	GridMeshGenTask(UGridMeshGen* _gridMeshGen, const FBlockGrid* _blockGrid, bool _bUpdateOnly);
	~GridMeshGenTask();

	void DoWork();


	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(ExampleAutoDeleteAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

};


class LOWPOLYSURVIVAL_API GridMeshGenThread : public FRunnable {

	static GridMeshGenThread* runnable;

	bool bKill = false;
	bool bPause = false;

	UGridMeshGen* gridMeshGen;
	const FBlockGrid* blockGrid;
	bool bUpdateOnly = false;

	FRunnableThread* Thread;

	TQueue<UGridMeshGen*> gridGens; 

public:
	GridMeshGenThread(UGridMeshGen* _gridMeshGen, const FBlockGrid* _blockGrid, bool _bUpdateOnly);
	~GridMeshGenThread();

	static GridMeshGenThread* JoyInt(UGridMeshGen* _gridMeshGen, const FBlockGrid* _blockGrid, bool _bUpdateOnly);

	//FRunnable interface.
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();

	void EnsureCompletion();
};