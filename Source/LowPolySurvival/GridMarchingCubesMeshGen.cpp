// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMarchingCubesMeshGen.h"
#include "MyGameInstance.h"
#include "Chunk.h"
#include "WorldGenerator.h"

FMeshGenData* UGridMarchingCubesMeshGen::CreateMesh(const FBlockGrid &blockGrid, const FGridMeshParams &param) {
	Super::CreateMesh(blockGrid, param);



	return &meshGenData;
}

FMeshGenData * UGridMarchingCubesMeshGen::UpdateMesh(const FBlockGrid & blockGrid, const FIntVector & blockLoc, const FGridMeshParams &param){
	Super::UpdateMesh(blockGrid, blockLoc, param);

	return &meshGenData;
}


void UGridMarchingCubesMeshGen::MarchingCubes(bool bBorderNormalsOnly) {

	meshGenData.Reset();

	//TArray<FVector> uniqueVertex; 
	meshVertexArray.Empty();
	meshTriangles.Empty();

	borderVertexIndecies.Empty();

	int32 borderSize = bBorderNormalsOnly ? 1 : 0;
	FVector cubeGridSize = FVector(marchCubes.Num(), marchCubes.IsValidIndex(0) ? marchCubes[0].Num() : 0, marchCubes[0].IsValidIndex(0) ? marchCubes[0][0].Num() : 0);

	//loop through cubeGrid without border
	for (size_t x = borderSize; x < cubeGridSize.X - borderSize; x++) {
		for (size_t y = borderSize; y < cubeGridSize.Y - borderSize; y++) {
			for (size_t z = 0; z < cubeGridSize.Z; z++) {
				uint8 cubeIndex = 0;

				//TODO remove this
				if (marchCubes[x][y].IsValidIndex(z) && marchCubes.IsValidIndex(x) && marchCubes[x].IsValidIndex(y)) {
					cubeIndex = marchCubes[x][y][z].GetCubeIndex();
				}

				//Loop through all Vertecies in a Cube 
				for (int32 i = triTable[cubeIndex].Num() - 1; i >= 0; --i) {
					uint8 edgeIndex = triTable[cubeIndex][i];

					FVector relEdgeCenter = relativEdgeCenters[edgeIndex];


					FVector vertex = BlockToWorldLocation(FVector(x - borderSize, y - borderSize, z) + relEdgeCenter);

					int32 vertIndex = meshGenData.vertexArray.Add(vertex);
					meshGenData.triangles.Add(vertIndex);

					int32 iVert = meshVertexArray.Add(vertex);
					meshTriangles.Add(iVert);
					//meshUV.Add(FVector2D(0.1, 0.1));


					meshGenData.vertColors.Add(marchCubes[x][y][z].GetEdgeColor(edgeIndex).ToFColor(true));


				}
			}
		}
	}


	//loop through cubeGride Border
	if (bBorderNormalsOnly) {
		for (size_t x = 0; x < cubeGridSize.X; x++) {
			for (size_t y = 0; y < cubeGridSize.Y; y++) {

				//check if index is border
				if (x == 0 || y == 0 || x == (cubeGridSize.X - 1) || y == (cubeGridSize.Y - 1)) {
					//UE_LOG(LogTemp, Warning, TEXT("loop: x:%i, y:%i"), x, y);

					for (size_t z = 0; z < cubeGridSize.Z; z++) {

						uint8 cubeIndex = 0;
						if (marchCubes[x][y].IsValidIndex(z)) {
							cubeIndex = marchCubes[x][y][z].GetCubeIndex();
						}
						else {
							UE_LOG(LogTemp, Warning, TEXT("zIndex: %i"), z);

						}


						//Loop through all Vertecies in a Cube
						for (int32 i = triTable[cubeIndex].Num() - 1; i >= 0; --i) {
							uint8 edgeIndex = triTable[cubeIndex][i];

							FVector relEdgeCenter = relativEdgeCenters[edgeIndex];

							FVector vertex = BlockToWorldLocation(FVector(x - borderSize, y - borderSize, z) + relEdgeCenter);

							uint32 vertIndex = meshGenData.vertexArray.Add(vertex);
							meshGenData.triangles.Add(vertIndex);

							if (meshVertexArray.Contains(vertex)) {
								meshVertexArray.Add(vertex);

								meshGenData.vertColors.Add(FColor::White);
							}
							else {
								borderVertexIndecies.Add(vertIndex);
							}


						}
					}
				}


			}
		}
	}

}

void UGridMarchingCubesMeshGen::CreateMarchCubes(const AChunk * chunk){
	
	if (!chunk) return;

	marchCubes.Reset();

	auto mainGrid = *chunk->GetGridData();


	//Initialize MarchCube 3D Array
	marchCubes.Init(TArray<TArray<FMarchCube>>(), gridSize.X);
	for (size_t x = 0; x < gridSize.X; x++) {
		marchCubes[x].Init(TArray<FMarchCube >(), gridSize.Y);

		for (size_t y = 0; y < gridSize.Y; y++) {
			marchCubes[x][y].Init(FMarchCube(), gridSize.Z);
			for (size_t z = 0; z < gridSize.Z; z++) {
				marchCubes[x][y][z].corners.Init(FBlockData(), 8);
			}
		}

	}

	for (size_t x = 0; x < gridSize.X - 1; x++) {
		for (size_t y = 0; y < gridSize.Y - 1; y++) {
			for (size_t z = 0; z < gridSize.Z - 1; z++) {

				marchCubes[x][y][z].corners[0] = mainGrid[x + 1][y][z].data;
				marchCubes[x][y][z].corners[1] = mainGrid[x + 1][y + 1][z].data;
				marchCubes[x][y][z].corners[2] = mainGrid[x][y + 1][z].data;
				marchCubes[x][y][z].corners[3] = mainGrid[x][y][z].data;
				marchCubes[x][y][z].corners[4] = mainGrid[x + 1][y][z + 1].data;
				marchCubes[x][y][z].corners[5] = mainGrid[x + 1][y + 1][z + 1].data;
				marchCubes[x][y][z].corners[6] = mainGrid[x][y + 1][z + 1].data;
				marchCubes[x][y][z].corners[7] = mainGrid[x][y][z + 1].data;
			}
		}
	}


	//Add MarchCubes from  neighbar chunks

	AWorldGenerator* worldGenerator = chunk->GetWorldGenerator();

	AChunk* forwardChunk = worldGenerator->GetChunkSafe(chunk->GetChunkLocation() + FIntVector(1, 0, 0));
	AChunk* rightChunk = worldGenerator->GetChunkSafe(chunk->GetChunkLocation() + FIntVector(0, 1, 0));
	AChunk* forwardRightChunk = worldGenerator->GetChunkSafe(chunk->GetChunkLocation() + FIntVector(1, 1, 0));

	AChunk* upChunk = worldGenerator->GetChunkSafe(chunk->GetChunkLocation() + FIntVector(0, 0, 1));
	AChunk* upForwardChunk = worldGenerator->GetChunkSafe(chunk->GetChunkLocation() + FIntVector(1, 0, 1));
	AChunk* upRightChunk = worldGenerator->GetChunkSafe(chunk->GetChunkLocation() + FIntVector(0, 1, 1));
	AChunk* upForwardRightChunk = worldGenerator->GetChunkSafe(chunk->GetChunkLocation() + FIntVector(1, 1, 1));


	if (forwardChunk) {

		size_t x = gridSize.X - 1;

		auto forwardGrid = *forwardChunk->GetGridData();

		for (size_t y = 0; y < gridSize.Y - 1; y++) {
			for (size_t z = 0; z < gridSize.Z - 1; z++) {
				marchCubes[x][y][z].corners[0] = forwardGrid[0][y][z].data;
				marchCubes[x][y][z].corners[1] = forwardGrid[0][y + 1][z].data;
				marchCubes[x][y][z].corners[2] = mainGrid[x][y + 1][z].data;
				marchCubes[x][y][z].corners[3] = mainGrid[x][y][z].data;
				marchCubes[x][y][z].corners[4] = forwardGrid[0][y][z + 1].data;
				marchCubes[x][y][z].corners[5] = forwardGrid[0][y + 1][z + 1].data;
				marchCubes[x][y][z].corners[6] = mainGrid[x][y + 1][z + 1].data;
				marchCubes[x][y][z].corners[7] = mainGrid[x][y][z + 1].data;
			}
		}
	}

	if (rightChunk) {

		size_t y = gridSize.Y - 1;

		auto rightGrid = *rightChunk->GetGridData();

		for (size_t x = 0; x < gridSize.X - 1; x++) {
			for (size_t z = 0; z < gridSize.Z - 1; z++) {
				marchCubes[x][y][z].corners[0] = mainGrid[x + 1][y][z].data;
				marchCubes[x][y][z].corners[1] = rightGrid[x + 1][0][z].data;
				marchCubes[x][y][z].corners[2] = rightGrid[x][0][z].data;
				marchCubes[x][y][z].corners[3] = mainGrid[x][y][z].data;
				marchCubes[x][y][z].corners[4] = mainGrid[x + 1][y][z + 1].data;
				marchCubes[x][y][z].corners[5] = rightGrid[x + 1][0][z + 1].data;
				marchCubes[x][y][z].corners[6] = rightGrid[x][0][z + 1].data;
				marchCubes[x][y][z].corners[7] = mainGrid[x][y][z + 1].data;
			}

		}
	}

	if (forwardRightChunk && forwardChunk && rightChunk) {

		size_t x = gridSize.X - 1;
		size_t y = gridSize.Y - 1;

		auto forwardGrid = *forwardChunk->GetGridData();
		auto rightGrid = *rightChunk->GetGridData();
		auto forwardRightGrid = *forwardRightChunk->GetGridData();


		for (size_t z = 0; z < gridSize.Z - 1; z++) {
			marchCubes[x][y][z].corners[0] = forwardGrid[0][y][z].data;
			marchCubes[x][y][z].corners[1] = forwardRightGrid[0][0][z].data;
			marchCubes[x][y][z].corners[2] = rightGrid[x][0][z].data;
			marchCubes[x][y][z].corners[3] = mainGrid[x][y][z].data;
			marchCubes[x][y][z].corners[4] = forwardGrid[0][y][z + 1].data;
			marchCubes[x][y][z].corners[5] = forwardRightGrid[0][0][z + 1].data;
			marchCubes[x][y][z].corners[6] = rightGrid[x][0][z + 1].data;
			marchCubes[x][y][z].corners[7] = mainGrid[x][y][z + 1].data;
		}
	}

	if (upChunk) {
		size_t z = gridSize.Z - 1;
		auto upGrid = *upChunk->GetGridData();

		for (size_t x = 0; x < gridSize.X - 1; x++) {
			for (size_t y = 0; y < gridSize.Y - 1; y++) {
				marchCubes[x][y][z].corners[0] = mainGrid[x + 1][y][z].data;
				marchCubes[x][y][z].corners[1] = mainGrid[x + 1][y + 1][z].data;
				marchCubes[x][y][z].corners[2] = mainGrid[x][y + 1][z].data;
				marchCubes[x][y][z].corners[3] = mainGrid[x][y][z].data;
				marchCubes[x][y][z].corners[4] = upGrid[x + 1][y][0].data;
				marchCubes[x][y][z].corners[5] = upGrid[x + 1][y + 1][0].data;
				marchCubes[x][y][z].corners[6] = upGrid[x][y + 1][0].data;
				marchCubes[x][y][z].corners[7] = upGrid[x][y][0].data;
			}
		}

	}

	if (upForwardChunk && upChunk && forwardChunk) {
		size_t x = gridSize.X - 1;
		size_t z = gridSize.Z - 1;

		auto forwardGrid = *forwardChunk->GetGridData();
		auto upGrid = *upChunk->GetGridData();
		auto upForwardGrid = *upForwardChunk->GetGridData();

		for (size_t y = 0; y < gridSize.Y - 1; y++) {
			marchCubes[x][y][z].corners[0] = forwardGrid[0][y][z].data;
			marchCubes[x][y][z].corners[1] = forwardGrid[0][y + 1][z].data;
			marchCubes[x][y][z].corners[2] = mainGrid[x][y + 1][z].data;
			marchCubes[x][y][z].corners[3] = mainGrid[x][y][z].data;
			marchCubes[x][y][z].corners[4] = upForwardGrid[0][y][0].data;
			marchCubes[x][y][z].corners[5] = upForwardGrid[0][y + 1][0].data;
			marchCubes[x][y][z].corners[6] = upGrid[x][y + 1][0].data;
			marchCubes[x][y][z].corners[7] = upGrid[x][y][0].data;
		}


	}

	if (upRightChunk && rightChunk && upChunk) {
		size_t y = gridSize.Y - 1;
		size_t z = gridSize.Z - 1;

		auto rightGrid = *rightChunk->GetGridData();
		auto upGrid = *upChunk->GetGridData();
		auto upRightGrid = *upRightChunk->GetGridData();

		for (size_t x = 0; x < gridSize.X - 1; x++) {
			marchCubes[x][y][z].corners[0] = mainGrid[x + 1][y][z].data;
			marchCubes[x][y][z].corners[1] = rightGrid[x + 1][0][z].data;
			marchCubes[x][y][z].corners[2] = rightGrid[x][0][z].data;
			marchCubes[x][y][z].corners[3] = mainGrid[x][y][z].data;
			marchCubes[x][y][z].corners[4] = upGrid[x + 1][y][0].data;
			marchCubes[x][y][z].corners[5] = upRightGrid[x + 1][0][0].data;
			marchCubes[x][y][z].corners[6] = upRightGrid[x][0][0].data;
			marchCubes[x][y][z].corners[7] = upGrid[x][y][0].data;
		}

	}

	if (forwardChunk && rightChunk && upChunk && upForwardChunk && upRightChunk && forwardRightChunk && upForwardRightChunk) {

		size_t x = gridSize.X - 1;
		size_t y = gridSize.Y - 1;
		size_t z = gridSize.Z - 1;

		marchCubes[x][y][z].corners[0] = (*forwardChunk)[0][y][z].data;
		marchCubes[x][y][z].corners[1] = (*forwardRightChunk)[0][0][z].data;
		marchCubes[x][y][z].corners[2] = (*rightChunk)[x][0][z].data;
		marchCubes[x][y][z].corners[3] = mainGrid[x][y][z].data;
		marchCubes[x][y][z].corners[4] = (*upForwardChunk)[0][y][0].data;
		marchCubes[x][y][z].corners[5] = (*upForwardRightChunk)[0][0][0].data;
		marchCubes[x][y][z].corners[6] = (*upRightChunk)[x][0][0].data;
		marchCubes[x][y][z].corners[7] = (*upChunk)[x][y][0].data;

	}
}

void UGridMarchingCubesMeshGen::DoTaskWork(bool bUpdateOnly, const FBlockGrid &blockGrid){
	Super::DoTaskWork(bUpdateOnly, blockGrid);

	if (!bUpdateOnly && params.chunk) {
		CreateMarchCubes(params.chunk);
	}

	MarchingCubes(false);
	UMeshGenerationLibrary::CalculateNormalsAndTangents(meshGenData, 0.0f);
	meshGenData.bIsReady = true;
}
