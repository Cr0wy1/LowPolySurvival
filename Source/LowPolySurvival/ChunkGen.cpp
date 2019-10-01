// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkGen.h"
#include "RunnableThread.h"
#include "MyGameInstance.h"
#include "Chunk.h"
#include "WorldGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "SaveRegion.h"
#include "GridNoiseGen.h"

ChunkGenThread* ChunkGenThread::runnable = nullptr;

ChunkGenThread::ChunkGenThread(AWorldGenerator* _worldGenerator) : worldGenerator(_worldGenerator){

	Thread = FRunnableThread::Create(this, TEXT("ChunkGenThread"), 0, EThreadPriority::TPri_AboveNormal);
}

ChunkGenThread::~ChunkGenThread() {

	if (Thread) {
		delete Thread;
		Thread = nullptr;
	}
}

ChunkGenThread * ChunkGenThread::JoyInt(AWorldGenerator* _worldGenerator) {

	if (!runnable) {
		runnable = new ChunkGenThread(_worldGenerator);
	}

	return runnable;
}

bool ChunkGenThread::Init() {

	blockGrid.Init(16, 16, 16);
	UE_LOG(LogTemp, Warning, TEXT("Thread: Init"));


	return true;
}

uint32 ChunkGenThread::Run() {

	FPlatformProcess::Sleep(0.03);

	while (!bKill) {
		

		if (!chunkDataQueue.IsEmpty()) {
			chunkDataQueue.Dequeue(cChunkGenData);

			//AChunk ** chunk = AWorldGenerator::loader.loadedChunks.Find(cChunkGenData.chunkLoc);

			if (cChunkGenData.bLoadChunk) {
				//if (!LoadFromFile()) {
					UE_LOG(LogTemp, Warning, TEXT("generate Noise"));

					cChunkGenData.chunk->GenerateNoise();
					cChunkGenData.chunk->RandomizeGrid(200, 10);
				//}
			}

			if (cChunkGenData.bGenerateMesh) {
				//cChunkGenData.chunk->GenerateTerrainMesh();
			}

			FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&]() {

				cChunkGenData.chunk->GenerateTerrainMesh();

			}, TStatId(), NULL, ENamedThreads::GameThread);
		}


	}

	return 0;
}

void ChunkGenThread::Stop() {

	bKill = true;

	//UE_LOG(LogTemp, Warning, TEXT("Thread: Stop"));

}

void ChunkGenThread::EnsureCompletion() {
	Stop();

	if (Thread) {
		Thread->WaitForCompletion();
	}
}

void ChunkGenThread::AddChunkGenData(FChunkGenData chunkGenData){
	chunkDataQueue.Enqueue(chunkGenData);
}

void ChunkGenThread::SaveToFile() {
	if (cChunkGenData.bSaveToFile) {
		FIntVector chunkLoc = cChunkGenData.chunk->chunkLoc;
		USaveRegion* saveRegion = Cast<USaveRegion>(UGameplayStatics::CreateSaveGameObject(USaveRegion::StaticClass()));
		saveRegion->SetGrid(blockGrid);
		FString slotName = worldGenerator->GetWorldName() + "/chunks/" + FString::FromInt(chunkLoc.X) + "." + FString::FromInt(chunkLoc.Y) + "." + FString::FromInt(chunkLoc.Z);
		UGameplayStatics::SaveGameToSlot(saveRegion, slotName, 0);

	}
}

bool ChunkGenThread::LoadFromFile() {
	FIntVector chunkLoc = cChunkGenData.chunk->chunkLoc;
	FString slotName = worldGenerator->GetWorldName() + "/chunks/" + FString::FromInt(chunkLoc.X) + "." + FString::FromInt(chunkLoc.Y) + "." + FString::FromInt(chunkLoc.Z);

	if (UGameplayStatics::DoesSaveGameExist(slotName, 0)) {
		USaveRegion* saveRegion = Cast<USaveRegion>(UGameplayStatics::LoadGameFromSlot(slotName, 0));
		if (saveRegion) {
			saveRegion->GetGrid(cChunkGenData.chunk, blockGrid);

			//gridDim = blockGrid.dims;

			return true;
		}
	}

	return false;
}
