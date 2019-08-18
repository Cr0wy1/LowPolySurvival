// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralMeshGeneratorComponent.h"
#include "DrawDebugHelpers.h"
#include "SimplexNoise.h"
#include "Chunk.h"
#include "KismetProceduralMeshLibrary.h"
#include "MyGameInstance.h"
#include "GridComponent.h"
#include "WorldGenerator.h"
#include "RunnableThread.h"




UProceduralMeshGeneratorComponent::UProceduralMeshGeneratorComponent(const FObjectInitializer & ObjectInitializer) : UProceduralMeshComponent(ObjectInitializer){

	PrimaryComponentTick.bCanEverTick = true;
	
}

void UProceduralMeshGeneratorComponent::BeginPlay(){
	Super::BeginPlay();

	//PrimaryComponentTick.SetTickFunctionEnable(true);

}

void UProceduralMeshGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (procMeshData.bIsReady && !bIsMeshGenerated) {
		CreateMesh(false);

	}
}


void UProceduralMeshGeneratorComponent::GenerateMesh(const AChunk * chunk){

	if (!chunk) return;

	procMeshData.bIsReady = false;
	bIsMeshGenerated = false;
	 
	auto chunkGrid = *chunk->GetGridData();
	gridSize = FVector(chunkGrid.Num(), chunkGrid.IsValidIndex(0) ? chunkGrid[0].Num() : 0, chunkGrid[0].IsValidIndex(0) ? chunkGrid[0][0].Num() : 0);

	//CreateMarchCubes(chunk);
	//UE_LOG(LogTemp, Warning, TEXT("x:%i, y:%i, z:%i"), marchCubes.Num(), marchCubes[0].Num(), marchCubes[0][0].Num());

	(new FAutoDeleteAsyncTask<ProcMeshTask>(this, chunk))->StartBackgroundTask();
	//MarchingCubes(false);
}

void UProceduralMeshGeneratorComponent::GenerateMesh(const TArray<TArray<TArray<FBlockData>>>& blockGrid){

	marchCubes.Empty();

	gridSize = FVector(blockGrid.Num(), blockGrid.IsValidIndex(0) ? blockGrid[0].Num() : 0, blockGrid[0].IsValidIndex(0) ? blockGrid[0][0].Num() : 0);

	CreateMarchCubes(blockGrid);
	MarchingCubes();
}

void UProceduralMeshGeneratorComponent::UpdateMesh(const AChunk * chunk, const FIntVector &chunkBlockLoc){
	
	if (!procMeshData.bIsReady) return;

	procMeshData.bIsReady = false;
	bIsMeshGenerated = false;

	int32 x = chunkBlockLoc.X;
	int32 y = chunkBlockLoc.Y;
	int32 z = chunkBlockLoc.Z;

	FBlockData blockData = chunk->GetBlock(chunkBlockLoc).data;

	(new FAutoDeleteAsyncTask<ProcMeshTask>(this, chunk))->StartBackgroundTask();

	//GenerateMesh(chunk);
}

void UProceduralMeshGeneratorComponent::UpdateMesh(const TArray<TArray<TArray<FBlockData>>>& blockGrid, FIntVector blockLocation) {
	ClearMeshSection(0);

	GenerateMesh(blockGrid);

}


void UProceduralMeshGeneratorComponent::CreateMarchCubes(const AChunk * chunk){

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

	const AWorldGenerator* worldGenerator = chunk->GetWorldGenerator();

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

void UProceduralMeshGeneratorComponent::CreateMarchCubes(const TArray<TArray<TArray<FBlockData>>>& blockGrid){

	marchCubes.Init(TArray<TArray<FMarchCube>>(), gridSize.X - 1);

	for (size_t x = 0; x < gridSize.X - 1; x++) {
		marchCubes[x].Init(TArray<FMarchCube >(), gridSize.Y - 1);

		for (size_t y = 0; y < gridSize.Y - 1; y++) {
			marchCubes[x][y].Init(FMarchCube(), gridSize.Z - 1);

			for (size_t z = 0; z < gridSize.Z - 1; z++) {

			
				marchCubes[x][y][z].corners.Add(blockGrid[x + 1][y][z]);
				marchCubes[x][y][z].corners.Add(blockGrid[x + 1][y + 1][z]);
				marchCubes[x][y][z].corners.Add(blockGrid[x][y + 1][z]); 
				marchCubes[x][y][z].corners.Add(blockGrid[x][y][z]);
				marchCubes[x][y][z].corners.Add(blockGrid[x + 1][y][z + 1]);
				marchCubes[x][y][z].corners.Add(blockGrid[x + 1][y + 1][z + 1]);
				marchCubes[x][y][z].corners.Add(blockGrid[x][y + 1][z + 1]);
				marchCubes[x][y][z].corners.Add(blockGrid[x][y][z + 1]);

			}
		}
	}

}

void UProceduralMeshGeneratorComponent::MarchingCubes(bool bBorderNormalsOnly){

	procMeshData.Reset();

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
				for (int32 i = triTable[cubeIndex].Num()-1; i >= 0; --i) {
					uint8 edgeIndex = triTable[cubeIndex][i];

					FVector relEdgeCenter = relativEdgeCenters[edgeIndex];


					FVector vertex = BlockToWorldLocation(FVector(x - borderSize, y - borderSize, z) + relEdgeCenter);

					int32 vertIndex = procMeshData.vertexArray.Add(vertex);
					procMeshData.triangles.Add(vertIndex);
						 
					int32 iVert = meshVertexArray.Add(vertex);
					meshTriangles.Add(iVert);
					//meshUV.Add(FVector2D(0.1, 0.1));
						
					
					procMeshData.vertColors.Add( marchCubes[x][y][z].GetEdgeColor(edgeIndex).ToFColor(true) );

					
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

							FVector vertex = BlockToWorldLocation(FVector(x - borderSize, y - borderSize, z)  + relEdgeCenter);

							uint32 vertIndex = procMeshData.vertexArray.Add(vertex);
							procMeshData.triangles.Add(vertIndex);

							if (meshVertexArray.Contains(vertex)) {
								meshVertexArray.Add(vertex);
								
								procMeshData.vertColors.Add(FColor::White);
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

	CalculateNormalsAndTangents(procMeshData);

}

void UProceduralMeshGeneratorComponent::CreateMesh(bool bBorderNormalsOnly){
	if (bBorderNormalsOnly) { 
		FOccluderVertexArray meshNormals;
		TArray<FProcMeshTangent> meshTangents;

		for (size_t i = 0; i < procMeshData.vertexArray.Num(); i++) {

			if (!borderVertexIndecies.Contains(i)) {
				meshNormals.Add(procMeshData.normals[i]);
				meshTangents.Add(procMeshData.tangents[i]);


				//Draw Normals
				//DrawDebugLine(GetWorld(), GetComponentLocation() + vertexArray[i], GetComponentLocation() + normals[i] * 100 + vertexArray[i], FColor::Red, false, 60, 0, 1);
			}

		}

		ClearMeshSection(0);
		CreateMeshSection(0, meshVertexArray, meshTriangles, meshNormals, TArray<FVector2D>(), procMeshData.vertColors, meshTangents, true);
	}
	else {
		ClearMeshSection(0);
		CreateMeshSection(0, procMeshData.vertexArray, procMeshData.triangles, procMeshData.normals, TArray<FVector2D>(), procMeshData.vertColors, procMeshData.tangents, true);
	}

	bIsMeshGenerated = true;
	onGeneratedMesh.Broadcast();
}


void FindVertOverlaps(int32 TestVertIndex, const TArray<FVector>& Verts, TArray<int32>& VertOverlaps)
{
	// Check if Verts is empty or test is outside range
	if (TestVertIndex < Verts.Num())
	{
		const FVector TestVert = Verts[TestVertIndex];

		for (int32 VertIdx = 0; VertIdx < Verts.Num(); VertIdx++)
		{
			// First see if we overlap, and smoothing groups are the same
			if (TestVert.Equals(Verts[VertIdx]))
			{
				// If it, so we are at least considered an 'overlap' for normal gen
				VertOverlaps.Add(VertIdx);
			}
		}
	}
}
 


void UProceduralMeshGeneratorComponent::CalculateNormalsAndTangents(FProcMeshData &procMeshData){
	 
	//TODO Makse it optional
	//return;
	float normalsMultiplyer = 0.0f;
	
	if (procMeshData.vertexArray.Num() == 0)
	{ 
		return;
	}

	// Number of triangles 
	const int32 NumTris = procMeshData.triangles.Num() / 3;
	// Number of verts
	const int32 NumVerts = procMeshData.vertexArray.Num();

	// Map of vertex to triangles in Triangles array
	TMultiMap<int32, int32> VertToTriMap;
	// Map of vertex to triangles to consider for normal calculation
	TMultiMap<int32, int32> VertToTriSmoothMap;

	// Normal/tangents for each face
	TArray<FVector> FaceTangentX, FaceTangentY, FaceTangentZ;
	FaceTangentX.AddUninitialized(NumTris);
	FaceTangentY.AddUninitialized(NumTris);
	FaceTangentZ.AddUninitialized(NumTris);

	// Iterate over triangles
	for (int TriIdx = 0; TriIdx < NumTris; TriIdx++)
	{
		int32 CornerIndex[3];
		FVector P[3];

		for (int32 CornerIdx = 0; CornerIdx < 3; CornerIdx++)
		{
			// Find vert index (clamped within range)
			int32 VertIndex = FMath::Min(procMeshData.triangles[(TriIdx * 3) + CornerIdx], NumVerts - 1);

			CornerIndex[CornerIdx] = VertIndex;
			P[CornerIdx] = procMeshData.vertexArray[VertIndex];

			// Find/add this vert to index buffer
			TArray<int32> VertOverlaps;
			FindVertOverlaps(VertIndex, procMeshData.vertexArray, VertOverlaps);

			// Remember which triangles map to this vert
			VertToTriMap.AddUnique(VertIndex, TriIdx);
			VertToTriSmoothMap.AddUnique(VertIndex, TriIdx);

			// Also update map of triangles that 'overlap' this vert (ie don't match UV, but do match smoothing) and should be considered when calculating normal
			for (int32 OverlapIdx = 0; OverlapIdx < VertOverlaps.Num(); OverlapIdx++)
			{
				// For each vert we overlap..
				int32 OverlapVertIdx = VertOverlaps[OverlapIdx];

				// Add this triangle to that vert
				VertToTriSmoothMap.AddUnique(OverlapVertIdx, TriIdx);

				// And add all of its triangles to us
				TArray<int32> OverlapTris;
				VertToTriMap.MultiFind(OverlapVertIdx, OverlapTris);
				for (int32 OverlapTriIdx = 0; OverlapTriIdx < OverlapTris.Num(); OverlapTriIdx++)
				{
					VertToTriSmoothMap.AddUnique(VertIndex, OverlapTris[OverlapTriIdx]);
				}
			}
		}

		// Calculate triangle edge vectors and normal
		const FVector Edge21 = P[1] - P[2];
		const FVector Edge20 = P[0] - P[2];
		const FVector TriNormal = (Edge21 ^ Edge20).GetSafeNormal();


		FaceTangentX[TriIdx] = Edge20.GetSafeNormal();
		FaceTangentY[TriIdx] = (FaceTangentX[TriIdx] ^ TriNormal).GetSafeNormal();
		

		FaceTangentZ[TriIdx] = TriNormal;
	}


	// Arrays to accumulate tangents into
	TArray<FVector> VertexTangentXSum, VertexTangentYSum, VertexTangentZSum;
	VertexTangentXSum.AddZeroed(NumVerts);
	VertexTangentYSum.AddZeroed(NumVerts);
	VertexTangentZSum.AddZeroed(NumVerts);

	// For each vertex..
	for (int VertxIdx = 0; VertxIdx < procMeshData.vertexArray.Num(); VertxIdx++)
	{
		// Find relevant triangles for normal
		TArray<int32> SmoothTris;
		VertToTriSmoothMap.MultiFind(VertxIdx, SmoothTris); 

		int32 mainTriIdx = *VertToTriMap.Find(VertxIdx); 
		VertexTangentZSum[VertxIdx] = FaceTangentZ[mainTriIdx];
		
		for (int i = 0; i < SmoothTris.Num(); i++) 
		{  
			int32 TriIdx = SmoothTris[i];

			if (TriIdx != mainTriIdx) {
				VertexTangentZSum[VertxIdx] += FaceTangentZ[TriIdx] * normalsMultiplyer;
			}
			
		} 
		
		  
		// Find relevant triangles for tangents  
		TArray<int32> TangentTris;
		VertToTriMap.MultiFind(VertxIdx, TangentTris); 
		 
		for (int i = 0; i < TangentTris.Num(); i++)
		{
			int32 TriIdx = TangentTris[i];
			VertexTangentXSum[VertxIdx] += FaceTangentX[TriIdx];
			VertexTangentYSum[VertxIdx] += FaceTangentY[TriIdx];
		}
	}

	// Finally, normalize tangents and build output arrays

	procMeshData.normals.Reset();
	procMeshData.normals.AddUninitialized(NumVerts);

	procMeshData.tangents.Reset();
	procMeshData.tangents.AddUninitialized(NumVerts);

	int32 innerIndex = 0;
	for (int VertxIdx = 0; VertxIdx < NumVerts; VertxIdx++)
	{
		
		//if (!borderVertexIndecies.Contains(VertxIdx)) {


			FVector& TangentX = VertexTangentXSum[VertxIdx];
			FVector& TangentY = VertexTangentYSum[VertxIdx];
			FVector& TangentZ = VertexTangentZSum[VertxIdx];

			TangentX.Normalize();
			TangentZ.Normalize();

			procMeshData.normals[VertxIdx] = TangentZ;

			// Use Gram-Schmidt orthogonalization to make sure X is orth with Z
			TangentX -= TangentZ * (TangentZ | TangentX);
			TangentX.Normalize();
			 
			// See if we need to flip TangentY when generating from cross product
			const bool bFlipBitangent = ((TangentZ ^ TangentX) | TangentY) < 0.f;

			procMeshData.tangents[VertxIdx] = FProcMeshTangent(TangentX, bFlipBitangent);

			innerIndex++;
		//}
	}
}





//##########################################################
//################### ProcMeshTask #########################
ProcMeshTask::ProcMeshTask(UProceduralMeshGeneratorComponent* _meshGenerator, const AChunk * _chunk, bool _bUpdateOnly = false){
	meshGenerator = _meshGenerator;
	chunk = _chunk;
	bUpdateOnly = _bUpdateOnly;
}

ProcMeshTask::~ProcMeshTask(){
	//UE_LOG(LogTemp, Warning, TEXT("ProcMeshTask: Destructor!"));
}

void ProcMeshTask::DoWork(){
	if (!bUpdateOnly) {
		meshGenerator->CreateMarchCubes(chunk);
	}
	
	meshGenerator->MarchingCubes(false);
	meshGenerator->procMeshData.bIsReady = true;
	//UE_LOG(LogTemp, Warning, TEXT("ProcMeshTask: DoWork!"));

}
