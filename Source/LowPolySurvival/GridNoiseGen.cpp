// Fill out your copyright notice in the Description page of Project Settings.


#include "GridNoiseGen.h"
#include "MyGameInstance.h"
#include "WorldGenerator.h"
#include "ChunkColumn.h"

void UGridNoiseGen::DoTaskWork(){


	auto terrainNoiseMap = *chunkColumn->GetTerrainNoiseMap();
	//TODO Testing, remove it
	for (int32 x = 0; x < blockGrid->dims.X; x++) {
		for (int32 y = 0; y < blockGrid->dims.Y; y++) {

			for (int32 z = 0; z < blockGrid->dims.Z; z++) {
				(*blockGrid)[x][y][z].data.noiseValue = Noise3D(x + blockOffset.X, y + blockOffset.Y, terrainNoiseMap[x][y] * 1000, 1, 1, 1000) * 0.85f;
				(*blockGrid)[x][y][z].data.noiseValue += Noise3D(x + blockOffset.X, y + blockOffset.Y, blockOffset.Z, 6, 4) * 0.15f;
				//blockGrid[x][y][z].data.noiseValue *= 0.5f;
			}
		}
	}


	ApplyNoiseOnGrid3D();
	AddNoiseCaves();

	bIsReady = true;

	FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&]() {
		OnFinishNoiseGen.Broadcast();
	}, TStatId(), NULL, ENamedThreads::GameThread);

	
}

void UGridNoiseGen::GenerateNoise(UMyGameInstance* _gameInstance, FBlockGrid * _blockGrid, UChunkColumn* _chunkColumn, FIntVector _blockOffset){
	gameInstance = _gameInstance;
	blockGrid = _blockGrid;
	blockOffset = _blockOffset;
	chunkColumn = _chunkColumn;

	task = new FAsyncTask<GridNoiseGenTask>(this);
	task->StartBackgroundTask();
}

void UGridNoiseGen::EnsureCompletion(){
	if (task) {
		task->EnsureCompletion();
		if (task->Cancel()) {
			delete task;
			task = NULL;
		}
	}
}

bool UGridNoiseGen::IsReady() const{
	return bIsReady;
}


void UGridNoiseGen::ApplyNoiseOnGrid3D() {

	AWorldGenerator* worldGenerator = gameInstance->GetWorldGenerator();

	UDataTable* resourceTable = gameInstance->GetResourceTable();
	FResource* dirtR = resourceTable->FindRow<FResource>("1", FString());
	FResource* grassR = resourceTable->FindRow<FResource>("2", FString());
	FResource* stoneR = resourceTable->FindRow<FResource>("3", FString());
	FResource* earthcoreR = resourceTable->FindRow<FResource>("4", FString());
	FResource* sandR = resourceTable->FindRow<FResource>("9", FString());
	FResource* snowR = resourceTable->FindRow<FResource>("10", FString());

	FBiomeData* cBiome;
	FResource* biomeR = grassR;

	auto terrainNoiseMap = *chunkColumn->GetTerrainNoiseMap();
	auto heatNoiseMap = *chunkColumn->GetHeatNoiseMap();
	auto rainNoiseMap = *chunkColumn->GetRainNoiseMap();
	auto hillsNoiseMap = *chunkColumn->GetHillsNoiseMap();


	for (int32 x = 0; x < blockGrid->dims.X; x++) {
		for (int32 y = 0; y < blockGrid->dims.Y; y++) {

			cBiome = worldGenerator->GetBiome(terrainNoiseMap[x][y], rainNoiseMap[x][y]);
			biomeR = cBiome->baseBlockResource;

			for (int32 z = 0; z < blockGrid->dims.Z; z++) {

				FBlock * cBlock = &(*blockGrid)[x][y][z];

				int32 upAmount = terrainNoiseMap[x][y] * FWorldParams::terrainHeight;
				int32 maxZ = FWorldParams::terrainBlockLevel + upAmount;

				int32 blockZ = blockOffset.Z + z;

				cBlock->data.noiseValue = cBlock->data.noiseValue*0.5f + 0.5f; // scaled to [0.5][1.0f]

				if (blockZ > FWorldParams::terrainBlockLevel) {
					cBlock->data.noiseValue *= 1.0f - (float)(blockZ - FWorldParams::terrainBlockLevel) / (FWorldParams::terrainHeight * 1);

				}

				if (cBlock->data.noiseValue > 0.5f) {
					//blockGrid[x][y][z].SetResource(dirtR);

					if (blockZ == 0) {
						cBlock->SetResource(earthcoreR);

					}
					else if (blockZ < (maxZ - 5)) {
						cBlock->SetResource(stoneR);

					}
					else if (blockZ < (maxZ - 2)) {
						cBlock->SetResource(dirtR);

					}
					else if (blockZ < (maxZ - 1)) {
						//Top Block
						cBlock->SetResource(biomeR);
						if (cBiome->overrideResourceColor) {
							cBlock->SetBiomeColor(cBiome->biomeColor);

						}
					}

					if (blockZ >(FWorldParams::terrainHeight + FWorldParams::terrainBlockLevel - 60)) {
						cBlock->SetResource(stoneR);
					}
					else if (blockZ > (FWorldParams::terrainHeight + FWorldParams::terrainBlockLevel - 67)) {
						cBlock->SetResource(dirtR);
					}
				}

			}


		}
	}
}

void UGridNoiseGen::AddNoiseCaves() {

	AWorldGenerator* worldGenerator = gameInstance->GetWorldGenerator();

	UDataTable* resourceTable = gameInstance->GetResourceTable();
	FResource* airR = resourceTable->FindRow<FResource>("0", FString());

	float caveNoiseLevel = 0.3f;

	//Check if is earthcore level to ensure not digging holes at this
	
	int32 zInit = blockGrid->dims.Z < 1 ? 1 : 0;

	for (int32 x = 0; x < blockGrid->dims.X; x++) {
		for (int32 y = 0; y < blockGrid->dims.Y; y++) {
			for (int32 z = zInit; z < blockGrid->dims.Z; z++) {

				float noise = worldGenerator->CaveNoise(FVector(blockOffset.X + x, blockOffset.Y + y, blockOffset.Z + z));



				//if (noise < caveNoiseLevel) { 
				//float lerpedNoise = FMath::Lerp(blockGrid[x][y][z].data.noiseValue, noise, 0.5f);
				noise *= 0.15f;

				(*blockGrid)[x][y][z].data.noiseValue -= noise;
				//blockGrid[x][y][z].SetResource(airR);

				//}
			}

		}
	}

}



//##### GridNoiseGenTask ######
GridNoiseGenTask::GridNoiseGenTask(UGridNoiseGen * _gridNoiseGen){
	gridNoiseGen = _gridNoiseGen;

}

GridNoiseGenTask::~GridNoiseGenTask(){
}

void GridNoiseGenTask::DoWork(){
	gridNoiseGen->DoTaskWork();
}


