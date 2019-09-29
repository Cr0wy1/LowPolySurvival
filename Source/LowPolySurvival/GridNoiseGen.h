// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HAL/Runnable.h"
#include "GameStructs.h"
#include "GridNoiseGen.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishNoiseGen);


class UMyGameInstance;
class UChunkColumn;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UGridNoiseGen : public UObject{
	GENERATED_BODY()

	friend class GridNoiseGenTask;

protected:

	bool bIsReady = false;

	UMyGameInstance * gameInstance;
	FBlockGrid* blockGrid;
	UChunkColumn* chunkColumn;
	FIntVector blockOffset;

	FAsyncTask<GridNoiseGenTask>* task;

	virtual void DoTaskWork();

	void ApplyNoiseOnGrid3D();
	void AddNoiseCaves();

public:

	FOnFinishNoiseGen OnFinishNoiseGen;

	void GenerateNoise(UMyGameInstance* _gameInstance, FBlockGrid* _blockGrid, UChunkColumn* _chunkColumn, FIntVector _blockOffset);

	virtual void EnsureCompletion();

	bool IsReady()const;
	
};


class GridNoiseGenTask : public FNonAbandonableTask {


	UGridNoiseGen* gridNoiseGen;

public:
	GridNoiseGenTask(UGridNoiseGen* _gridNoiseGen);
	~GridNoiseGenTask();

	void DoWork();


	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(ExampleAutoDeleteAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

};
