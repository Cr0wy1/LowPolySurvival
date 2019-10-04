// Fill out your copyright notice in the Description page of Project Settings.


#include "GridNoiseGen.h"
#include "MyGameInstance.h"
#include "WorldGenerator.h"
#include "ChunkColumn.h"

void UGridNoiseGen::GenerateNoiseS(UMyGameInstance* _gameInstance, FBlockGrid* _blockGrid, UChunkColumn* _chunkColumn, FIntVector _blockOffset, bool bThreading) {

}

void UGridNoiseGen::DoTaskWork(){

	auto terrainNoiseMap = *chunkColumn->GetTerrainNoiseMap();
	//TODO Testing, remove it
	for (int32 x = 0; x < blockGrid->dims.X; x++) {
		for (int32 y = 0; y < blockGrid->dims.Y; y++) {
			for (int32 z = 0; z < blockGrid->dims.Z; z++) {
				float noiseValue = 0.0f;
				//bufferGrid[x][y][z].data.noiseValue = Noise3D(x + blockOffset.X, y + blockOffset.Y, z + blockOffset.Z, 1, 1.0f, 1);
				noiseValue = Noise3D(x + blockOffset.X, y + blockOffset.Y, z + blockOffset.Z, 3, 1.0f, 1); //flat base layer																												  //to make sure under terrainBlockLevel is not air
				noiseValue = noiseValue *0.5f + 0.5f; // scaled to [0.5][1.0f]

				//(*blockGrid)[x][y][z].data.noiseValue += FMath::Pow( Noise3D(x + blockOffset.X, y + blockOffset.Y, z + blockOffset.Z, 1, 2.0f, 1), 2) * -0.5f;
				//(*blockGrid)[x][y][z].data.noiseValue = Noise3D(x + blockOffset.X, y + blockOffset.Y, terrainNoiseMap[x][y] * 1000, 1, 0.1f, 1000) * 0.85f;
				//(*blockGrid)[x][y][z].data.noiseValue += Noise3D(x + blockOffset.X, y + blockOffset.Y, blockOffset.Z, 6, 4) * 0.15f;
				//blockGrid[x][y][z].data.noiseValue *= 0.5f;

				(*blockGrid)[x][y][z].data.noiseValue = noiseValue;
			}
		}
	}
	

	ApplyNoiseOnGrid3D();
	//AddNoiseCaves();
	//AddNoiseOres();
	AddWater();

	bIsReady = true;

	FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&]() {
		OnFinishNoiseGen.Broadcast();
	}, TStatId(), NULL, ENamedThreads::GameThread);

	
}

void UGridNoiseGen::GenerateNoise(UMyGameInstance* _gameInstance, FBlockGrid * _blockGrid, UChunkColumn* _chunkColumn, FIntVector _blockOffset, bool bThreading) {
	gameInstance = _gameInstance;
	blockGrid = _blockGrid;
	blockOffset = _blockOffset;
	chunkColumn = _chunkColumn;
	worldGenerator = gameInstance->GetWorldGenerator();

	bufferGrid.Init(16, 16, 16);

	if (bThreading) {
		task = new FAsyncTask<GridNoiseGenTask>(this);
		task->StartBackgroundTask();
	}
	else {
		DoTaskWork();
	}

}

void UGridNoiseGen::EnsureCompletion(){
	//UE_LOG(LogTemp, Warning, TEXT("Ensure completation"));

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

	FBiomeData* cBiome;
	FResource* biomeR = FSResource::GRASS;

	auto terrainNoiseMap = *chunkColumn->GetTerrainNoiseMap();
	auto heatNoiseMap = *chunkColumn->GetHeatNoiseMap();
	auto rainNoiseMap = *chunkColumn->GetRainNoiseMap();
	auto hillsNoiseMap = *chunkColumn->GetHillsNoiseMap();


	for (int32 x = 0; x < blockGrid->dims.X; x++) {
		for (int32 y = 0; y < blockGrid->dims.Y; y++) {

			cBiome = worldGenerator->GetBiome(terrainNoiseMap[x][y], rainNoiseMap[x][y]);
			biomeR = cBiome->baseBlockResource;

			float upAmount = terrainNoiseMap[x][y] * FWorldParams::terrainHeight;
			float maxZ = upAmount + FWorldParams::terrainBlockLevel;

			for (int32 z = 0; z < blockGrid->dims.Z; z++) {

				FBlock * cBlock = &(*blockGrid)[x][y][z];
				int32 blockZ = blockOffset.Z + z;


				float deltaBlockZ = blockZ - FWorldParams::terrainBlockLevel;

				float heightPercent = deltaBlockZ / upAmount;
				if (upAmount <= 0.0f) {
					heightPercent = (deltaBlockZ-upAmount ) / FMath::Abs(upAmount);
					
				}

				//2D terrainNoiseMap gen
				//if (blockZ > maxZ) {
					//cBlock->SetResource(FSResource::STONE);
					//cBlock->data.noiseValue = 0.0f;
				//}

				cBlock->data.noiseValue *= 1.0f - heightPercent;


				if (cBlock->data.noiseValue > 0.5f) {
					//blockGrid[x][y][z].SetResource(dirtR);
					//cBlock->SetResource(FSResource::STONE);
					//return;
					if (blockZ == 0) {
						cBlock->SetResource(FSResource::EARTHCORE);

					}
					else if (blockZ < (maxZ - 5)) {
						cBlock->SetResource(FSResource::STONE);

					}
					else if (blockZ < (maxZ - 2)) {
						cBlock->SetResource(FSResource::DIRT);

					}
					else if (blockZ < (maxZ - 1)) {
						//Top Block
						cBlock->SetResource(biomeR);
						if (cBiome->overrideResourceColor) {
							cBlock->SetBiomeColor(cBiome->biomeColor);

						}
						
					}

					cBlock->SetResource(FSResource::GRASS);

					if (blockZ >(FWorldParams::terrainHeight + FWorldParams::terrainBlockLevel - 60)) {
						cBlock->SetResource(FSResource::STONE);
					}
					else if (blockZ > (FWorldParams::terrainHeight + FWorldParams::terrainBlockLevel - 67)) {
						cBlock->SetResource(FSResource::DIRT);
					}
				}

			}


		}
	}
}

void UGridNoiseGen::AddNoiseCaves() {
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

void UGridNoiseGen::AddNoiseOres() {

	float oreNoiseLevel = 0.06f;

	for (int32 x = 0; x < blockGrid->dims.X; x++) {
		for (int32 y = 0; y < blockGrid->dims.Y; y++) {
			for (int32 z = 0; z < blockGrid->dims.Z; z++) {

				FBlock * cBlock = &(*blockGrid)[x][y][z];

				if (cBlock->resource && cBlock->resource->id == 3) {

					float noiseCoal = worldGenerator->OreNoise(FVector(blockOffset.X + x, blockOffset.Y + y, blockOffset.Z + z));
					float noiseIron = worldGenerator->OreNoise(FVector(blockOffset.X + x * 2, blockOffset.Y + y * 2, blockOffset.Z + z * 2));
					float noiseGold = worldGenerator->OreNoise(FVector(blockOffset.X + x * 3, blockOffset.Y + y * 3, blockOffset.Z + z * 3));

					if (noiseCoal < 0.08f) {
						cBlock->SetResource(FSResource::COAL);
					}
					if (noiseIron < 0.04) {
						cBlock->SetResource(FSResource::IRON);
					}
					if (noiseGold < 0.02) {
						cBlock->SetResource(FSResource::GOLD);
					}
				}
			}

		}
	}
}

void UGridNoiseGen::AddWater() {
	auto terrainNoiseMap = *chunkColumn->GetTerrainNoiseMap();

	for (int32 x = 0; x < blockGrid->dims.X; x++) {
		for (int32 y = 0; y < blockGrid->dims.Y; y++) {

			for (int32 z = 0; z < blockGrid->dims.Z; z++) {

				int32 blockZ = blockOffset.Z + z;

				if (blockZ <= 100 && (*blockGrid)[x][y][z].IsAir()) {
					(*blockGrid)[x][y][z].SetResource(FSResource::WATER);
					(*blockGrid)[x][y][z].data.noiseValue = 1.0f;
				}
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


