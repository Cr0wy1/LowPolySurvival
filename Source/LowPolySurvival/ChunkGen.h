// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HAL/Runnable.h"
#include "Queue.h"
#include "GameStructs.h"
#include "ChunkGen.generated.h"

/**
 * 
 */
class AWorldGenerator;
class AChunk;



USTRUCT()
struct LOWPOLYSURVIVAL_API FChunkGenData {
	GENERATED_BODY()

	AChunk* chunk;
	bool bLoadChunk = false;
	bool bGenerateMesh = false;
	bool bSaveToFile = false;
};



UCLASS()
class LOWPOLYSURVIVAL_API UChunkGen : public UObject
{
	GENERATED_BODY()
	
};

class LOWPOLYSURVIVAL_API ChunkGenThread : public FRunnable {

	bool bKill = false;

	static ChunkGenThread* runnable;

	FRunnableThread* Thread;

	TQueue<FChunkGenData> chunkDataQueue;

	AWorldGenerator* worldGenerator;
	AChunk* chunk;

	FChunkGenData cChunkGenData;
	FBlockGrid blockGrid;

public:
	ChunkGenThread(AWorldGenerator* _worldGenerator);
	~ChunkGenThread();

	static ChunkGenThread* JoyInt(AWorldGenerator* _worldGenerator);

	//FRunnable interface.
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();

	void EnsureCompletion();

	void AddChunkGenData(FChunkGenData chunkGenData);

protected:

	void SaveToFile();
	bool LoadFromFile();
};
