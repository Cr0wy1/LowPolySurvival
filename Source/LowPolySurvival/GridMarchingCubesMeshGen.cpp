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
					cubeIndex = marchCubes[x][y][z].GetCubeIndex(isolevel);
				}

				//Loop through all Vertecies in a Cube 
				for (int32 i = FMarchCube::triTable[cubeIndex].Num() - 1; i >= 0; --i) {
					uint8 edgeIndex = FMarchCube::triTable[cubeIndex][i];

					FVector relEdgeCenter = FMarchCube::relativEdgeCenters[edgeIndex];
					FVector relVertexPos =  marchCubes[x][y][z].GetLerpedVertexpPos(edgeIndex);

					FVector vertex = BlockToWorldLocation(FVector(x - borderSize, y - borderSize, z) + relVertexPos);

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
							cubeIndex = marchCubes[x][y][z].GetCubeIndex(isolevel);
						}
						else {
							UE_LOG(LogTemp, Warning, TEXT("zIndex: %i"), z);

						}


						//Loop through all Vertecies in a Cube
						for (int32 i = FMarchCube::triTable[cubeIndex].Num() - 1; i >= 0; --i) {
							uint8 edgeIndex = FMarchCube::triTable[cubeIndex][i];

							FVector relEdgeCenter = FMarchCube::relativEdgeCenters[edgeIndex];

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


//FMarchCube
uint8 FMarchCube::GetCubeIndex(float isolevel) const{
	uint8 cubeIndex = 0;
	for (size_t i = 0; i < 8; i++) {
		if (corners[i].noiseValue < isolevel) {
			cubeIndex |= 1 << i;
		}
	}
	return cubeIndex;
}


FLinearColor FMarchCube::GetEdgeColor(uint8 edgeIndex) const {
	{

		uint8 cornerIndex1 = edgeCorners[edgeIndex][0];
		uint8 cornerIndex2 = edgeCorners[edgeIndex][1];

		const FBlockData* corner1 = &corners[cornerIndex1];
		const FBlockData* corner2 = &corners[cornerIndex2];

		FLinearColor finalColor;

		if (corner1->bIsSolid) {
			finalColor = corner1->color;

			if (corner2->bIsSolid) {
				FLinearColor color2 = corner2->color;
				finalColor = FLinearColor::LerpUsingHSV(finalColor, color2, 0.5f);
			}
		}
		else {
			finalColor = corner2->color;
		}

		return finalColor;
	}
}

FVector FMarchCube::GetLerpedVertexpPos(uint8 edgeIndex) const{

	FVector result;

	uint8 cornerIndex1 = edgeCorners[edgeIndex][0];
	uint8 cornerIndex2 = edgeCorners[edgeIndex][1];

	//float scalar = FMath::Lerp(corners[cornerIndex1].noiseValue, corners[cornerIndex2].noiseValue, 0.5f);
	//return FMath::Lerp(relativCorners[cornerIndex1], relativCorners[cornerIndex2], scalar);
	FVector p1 = relativCorners[cornerIndex1];
	FVector p2 = relativCorners[cornerIndex2];
	
	float valp1 = corners[cornerIndex1].noiseValue;
	float valp2 = corners[cornerIndex2].noiseValue;

	float isolevel = 0.5f;

	if (FMath::Abs(isolevel - valp1) < 0.00001)
		return(p1);
	if (FMath::Abs(isolevel - valp2) < 0.00001)
		return(p2);
	if (FMath::Abs(valp1 - valp2) < 0.00001)
		return(p1);

	

	
	float mu = (isolevel - valp1) / (valp2 - valp1);
	result.X = p1.X + mu * (p2.X - p1.X);
	result.Y = p1.Y + mu * (p2.Y - p1.Y);
	result.Z = p1.Z + mu * (p2.Z - p1.Z);

	return result;
}

//MarchingCube LookupTables

const TArray<TArray<uint8>> FMarchCube::edgeCorners = {
	{ 0,1 },
	{ 2,1 },
	{ 3,2 },
	{ 3,0 },
	{ 4,5 },
	{ 6,5 },
	{ 7,6 },
	{ 7,4 },
	{ 0,4 },
	{ 1,5 },
	{ 2,6 },
	{ 3,7 },
};

const TArray<FVector> FMarchCube::relativEdgeCenters = {
	FVector(0.5, 0.0,-0.5),
	FVector(0.0, 0.5,-0.5),
	FVector(-0.5, 0.0,-0.5),
	FVector(0.0,-0.5,-0.5),
	FVector(0.5, 0.0, 0.5),
	FVector(0.0, 0.5, 0.5),
	FVector(-0.5, 0.0, 0.5),
	FVector(0.0,-0.5, 0.5),
	FVector(0.5,-0.5, 0.0),
	FVector(0.5, 0.5, 0.0),
	FVector(-0.5, 0.5, 0.0),
	FVector(-0.5,-0.5, 0.0),
};

const TArray<FVector> FMarchCube::relativCorners = {
	FVector(0.5, -0.5, -0.5),
	FVector(0.5, 0.5, -0.5),
	FVector(-0.5, 0.5, -0.5),
	FVector(-0.5, -0.5, -0.5),
	FVector(0.5, -0.5, 0.5),
	FVector(0.5, 0.5, 0.5),
	FVector(-0.5, 0.5, 0.5),
	FVector(-0.5, -0.5, 0.5),
};

const TArray<TArray<uint8>> FMarchCube::triTable =
{
	{},
{ 0, 8, 3 },
{ 0, 1, 9 },
{ 1, 8, 3, 9, 8, 1 },
{ 1, 2, 10 },
{ 0, 8, 3, 1, 2, 10 },
{ 9, 2, 10, 0, 2, 9 },
{ 2, 8, 3, 2, 10, 8, 10, 9, 8 },
{ 3, 11, 2 },
{ 0, 11, 2, 8, 11, 0 },
{ 1, 9, 0, 2, 3, 11 },
{ 1, 11, 2, 1, 9, 11, 9, 8, 11 },
{ 3, 10, 1, 11, 10, 3 },
{ 0, 10, 1, 0, 8, 10, 8, 11, 10 },
{ 3, 9, 0, 3, 11, 9, 11, 10, 9 },
{ 9, 8, 10, 10, 8, 11 },
{ 4, 7, 8 },
{ 4, 3, 0, 7, 3, 4 },
{ 0, 1, 9, 8, 4, 7 },
{ 4, 1, 9, 4, 7, 1, 7, 3, 1 },
{ 1, 2, 10, 8, 4, 7 },
{ 3, 4, 7, 3, 0, 4, 1, 2, 10 },
{ 9, 2, 10, 9, 0, 2, 8, 4, 7 },
{ 2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4 },
{ 8, 4, 7, 3, 11, 2 },
{ 11, 4, 7, 11, 2, 4, 2, 0, 4 },
{ 9, 0, 1, 8, 4, 7, 2, 3, 11 },
{ 4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1 },
{ 3, 10, 1, 3, 11, 10, 7, 8, 4 },
{ 1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4 },
{ 4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3 },
{ 4, 7, 11, 4, 11, 9, 9, 11, 10 },
{ 9, 5, 4 },
{ 9, 5, 4, 0, 8, 3 },
{ 0, 5, 4, 1, 5, 0 },
{ 8, 5, 4, 8, 3, 5, 3, 1, 5 },
{ 1, 2, 10, 9, 5, 4 },
{ 3, 0, 8, 1, 2, 10, 4, 9, 5 },
{ 5, 2, 10, 5, 4, 2, 4, 0, 2 },
{ 2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8 },
{ 9, 5, 4, 2, 3, 11 },
{ 0, 11, 2, 0, 8, 11, 4, 9, 5 },
{ 0, 5, 4, 0, 1, 5, 2, 3, 11 },
{ 2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5 },
{ 10, 3, 11, 10, 1, 3, 9, 5, 4 },
{ 4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10 },
{ 5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3 },
{ 5, 4, 8, 5, 8, 10, 10, 8, 11 },
{ 9, 7, 8, 5, 7, 9 },
{ 9, 3, 0, 9, 5, 3, 5, 7, 3 },
{ 0, 7, 8, 0, 1, 7, 1, 5, 7 },
{ 1, 5, 3, 3, 5, 7 },
{ 9, 7, 8, 9, 5, 7, 10, 1, 2 },
{ 10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3 },
{ 8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2 },
{ 2, 10, 5, 2, 5, 3, 3, 5, 7 },
{ 7, 9, 5, 7, 8, 9, 3, 11, 2 },
{ 9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11 },
{ 2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7 },
{ 11, 2, 1, 11, 1, 7, 7, 1, 5 },
{ 9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11 },
{ 5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0 },
{ 11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0 },
{ 11, 10, 5, 7, 11, 5 },
{ 10, 6, 5 },
{ 0, 8, 3, 5, 10, 6 },
{ 9, 0, 1, 5, 10, 6 },
{ 1, 8, 3, 1, 9, 8, 5, 10, 6 },
{ 1, 6, 5, 2, 6, 1 },
{ 1, 6, 5, 1, 2, 6, 3, 0, 8 },
{ 9, 6, 5, 9, 0, 6, 0, 2, 6 },
{ 5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8 },
{ 2, 3, 11, 10, 6, 5 },
{ 11, 0, 8, 11, 2, 0, 10, 6, 5 },
{ 0, 1, 9, 2, 3, 11, 5, 10, 6 },
{ 5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11 },
{ 6, 3, 11, 6, 5, 3, 5, 1, 3 },
{ 0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6 },
{ 3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9 },
{ 6, 5, 9, 6, 9, 11, 11, 9, 8 },
{ 5, 10, 6, 4, 7, 8 },
{ 4, 3, 0, 4, 7, 3, 6, 5, 10 },
{ 1, 9, 0, 5, 10, 6, 8, 4, 7 },
{ 10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4 },
{ 6, 1, 2, 6, 5, 1, 4, 7, 8 },
{ 1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7 },
{ 8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6 },
{ 7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9 },
{ 3, 11, 2, 7, 8, 4, 10, 6, 5 },
{ 5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11 },
{ 0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6 },
{ 9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6 },
{ 8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6 },
{ 5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11 },
{ 0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7 },
{ 6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9 },
{ 10, 4, 9, 6, 4, 10 },
{ 4, 10, 6, 4, 9, 10, 0, 8, 3 },
{ 10, 0, 1, 10, 6, 0, 6, 4, 0 },
{ 8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10 },
{ 1, 4, 9, 1, 2, 4, 2, 6, 4 },
{ 3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4 },
{ 0, 2, 4, 4, 2, 6 },
{ 8, 3, 2, 8, 2, 4, 4, 2, 6 },
{ 10, 4, 9, 10, 6, 4, 11, 2, 3 },
{ 0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6 },
{ 3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10 },
{ 6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1 },
{ 9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3 },
{ 8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1 },
{ 3, 11, 6, 3, 6, 0, 0, 6, 4 },
{ 6, 4, 8, 11, 6, 8 },
{ 7, 10, 6, 7, 8, 10, 8, 9, 10 },
{ 0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10 },
{ 10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0 },
{ 10, 6, 7, 10, 7, 1, 1, 7, 3 },
{ 1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7 },
{ 2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9 },
{ 7, 8, 0, 7, 0, 6, 6, 0, 2 },
{ 7, 3, 2, 6, 7, 2 },
{ 2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7 },
{ 2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7 },
{ 1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11 },
{ 11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1 },
{ 8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6 },
{ 0, 9, 1, 11, 6, 7 },
{ 7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0 },
{ 7, 11, 6 },
{ 7, 6, 11 },
{ 3, 0, 8, 11, 7, 6 },
{ 0, 1, 9, 11, 7, 6 },
{ 8, 1, 9, 8, 3, 1, 11, 7, 6 },
{ 10, 1, 2, 6, 11, 7 },
{ 1, 2, 10, 3, 0, 8, 6, 11, 7 },
{ 2, 9, 0, 2, 10, 9, 6, 11, 7 },
{ 6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8 },
{ 7, 2, 3, 6, 2, 7 },
{ 7, 0, 8, 7, 6, 0, 6, 2, 0 },
{ 2, 7, 6, 2, 3, 7, 0, 1, 9 },
{ 1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6 },
{ 10, 7, 6, 10, 1, 7, 1, 3, 7 },
{ 10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8 },
{ 0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7 },
{ 7, 6, 10, 7, 10, 8, 8, 10, 9 },
{ 6, 8, 4, 11, 8, 6 },
{ 3, 6, 11, 3, 0, 6, 0, 4, 6 },
{ 8, 6, 11, 8, 4, 6, 9, 0, 1 },
{ 9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6 },
{ 6, 8, 4, 6, 11, 8, 2, 10, 1 },
{ 1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6 },
{ 4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9 },
{ 10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3 },
{ 8, 2, 3, 8, 4, 2, 4, 6, 2 },
{ 0, 4, 2, 4, 6, 2 },
{ 1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8 },
{ 1, 9, 4, 1, 4, 2, 2, 4, 6 },
{ 8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1 },
{ 10, 1, 0, 10, 0, 6, 6, 0, 4 },
{ 4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3 },
{ 10, 9, 4, 6, 10, 4 },
{ 4, 9, 5, 7, 6, 11 },
{ 0, 8, 3, 4, 9, 5, 11, 7, 6 },
{ 5, 0, 1, 5, 4, 0, 7, 6, 11 },
{ 11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5 },
{ 9, 5, 4, 10, 1, 2, 7, 6, 11 },
{ 6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5 },
{ 7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2 },
{ 3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6 },
{ 7, 2, 3, 7, 6, 2, 5, 4, 9 },
{ 9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7 },
{ 3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0 },
{ 6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8 },
{ 9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7 },
{ 1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4 },
{ 4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10 },
{ 7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10 },
{ 6, 9, 5, 6, 11, 9, 11, 8, 9 },
{ 3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5 },
{ 0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11 },
{ 6, 11, 3, 6, 3, 5, 5, 3, 1 },
{ 1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6 },
{ 0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10 },
{ 11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5 },
{ 6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3 },
{ 5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2 },
{ 9, 5, 6, 9, 6, 0, 0, 6, 2 },
{ 1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8 },
{ 1, 5, 6, 2, 1, 6 },
{ 1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6 },
{ 10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0 },
{ 0, 3, 8, 5, 6, 10 },
{ 10, 5, 6 },
{ 11, 5, 10, 7, 5, 11 },
{ 11, 5, 10, 11, 7, 5, 8, 3, 0 },
{ 5, 11, 7, 5, 10, 11, 1, 9, 0 },
{ 10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1 },
{ 11, 1, 2, 11, 7, 1, 7, 5, 1 },
{ 0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11 },
{ 9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7 },
{ 7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2 },
{ 2, 5, 10, 2, 3, 5, 3, 7, 5 },
{ 8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5 },
{ 9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2 },
{ 9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2 },
{ 1, 3, 5, 3, 7, 5 },
{ 0, 8, 7, 0, 7, 1, 1, 7, 5 },
{ 9, 0, 3, 9, 3, 5, 5, 3, 7 },
{ 9, 8, 7, 5, 9, 7 },
{ 5, 8, 4, 5, 10, 8, 10, 11, 8 },
{ 5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0 },
{ 0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5 },
{ 10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4 },
{ 2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8 },
{ 0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11 },
{ 0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5 },
{ 9, 4, 5, 2, 11, 3 },
{ 2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4 },
{ 5, 10, 2, 5, 2, 4, 4, 2, 0 },
{ 3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9 },
{ 5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2 },
{ 8, 4, 5, 8, 5, 3, 3, 5, 1 },
{ 0, 4, 5, 1, 0, 5 },
{ 8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5 },
{ 9, 4, 5 },
{ 4, 11, 7, 4, 9, 11, 9, 10, 11 },
{ 0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11 },
{ 1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11 },
{ 3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4 },
{ 4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2 },
{ 9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3 },
{ 11, 7, 4, 11, 4, 2, 2, 4, 0 },
{ 11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4 },
{ 2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9 },
{ 9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7 },
{ 3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10 },
{ 1, 10, 2, 8, 7, 4 },
{ 4, 9, 1, 4, 1, 7, 7, 1, 3 },
{ 4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1 },
{ 4, 0, 3, 7, 4, 3 },
{ 4, 8, 7 },
{ 9, 10, 8, 10, 11, 8 },
{ 3, 0, 9, 3, 9, 11, 11, 9, 10 },
{ 0, 1, 10, 0, 10, 8, 8, 10, 11 },
{ 3, 1, 10, 11, 3, 10 },
{ 1, 2, 11, 1, 11, 9, 9, 11, 8 },
{ 3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9 },
{ 0, 2, 11, 8, 0, 11 },
{ 3, 2, 11 },
{ 2, 3, 8, 2, 8, 10, 10, 8, 9 },
{ 9, 10, 2, 0, 9, 2 },
{ 2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8 },
{ 1, 10, 2 },
{ 1, 3, 8, 9, 1, 8 },
{ 0, 9, 1 },
{ 0, 3, 8 },
{}
};


