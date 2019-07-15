// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralMeshGeneratorComponent.h"
#include "DrawDebugHelpers.h"
#include "SimplexNoise.h"
#include "Chunk.h"
#include "KismetProceduralMeshLibrary.h"
#include "MyGameInstance.h"
#include "GridComponent.h"


UProceduralMeshGeneratorComponent::UProceduralMeshGeneratorComponent(const FObjectInitializer & ObjectInitializer) : UProceduralMeshComponent(ObjectInitializer){


}


void UProceduralMeshGeneratorComponent::GenerateMesh(const UGridComponent * gridComp){
	 
	GenerateMesh(*gridComp->GetGridPointer()); 
}

void UProceduralMeshGeneratorComponent::GenerateMesh(const TArray<TArray<TArray<FBlockInfo>>>& blockGrid){

	marchCubes.Empty();

	gridSize = FVector(blockGrid.Num(), blockGrid.IsValidIndex(0) ? blockGrid[0].Num() : 0, blockGrid[0].IsValidIndex(0) ? blockGrid[0][0].Num() : 0);

	CreateMarchCubes(blockGrid);
	MarchingCubes();
}

void UProceduralMeshGeneratorComponent::UpdateMesh(const TArray<TArray<TArray<FBlockInfo>>>& blockGrid, FIntVector blockLocation) {
	ClearMeshSection(0);

	GenerateMesh(blockGrid);
	/*marchCubes[blockLocation.X][blockLocation.Y][blockLocation.Z].holdingVertexIndecies


	int32 borderSize = 1;
	FVector cubeGridSize = FVector(gridSize.X - 1, gridSize.Y - 1, gridSize.Z - 1);

	//loop through cubeGrid without border
	for (size_t x = borderSize; x < cubeGridSize.X - borderSize; x++) {
		for (size_t y = borderSize; y < cubeGridSize.Y - borderSize; y++) {

			for (size_t z = 0; z < cubeGridSize.Z; z++) {
				uint8 cubeIndex = marchCubes[x][y][z].GetCubeIndex(surfaceLevel);

				//Loop through all Vertecies in a Cube 
				for (int32 i = triTable[cubeIndex].Num() - 1; i >= 0; --i) {
					uint8 edgeIndex = triTable[cubeIndex][i];

					FVector relEdgeCenter = relativEdgeCenters[edgeIndex];


					FVector vertex = BlockToWorldLocation(FVector(x - borderSize, y - borderSize, z) + relEdgeCenter) + FVector(50, 50, 50);

					int32 vertIndex = vertexArray.Add(vertex);
					triangles.Add(vertIndex);
					marchCubes[x][y][z].holdingVertexIndecies.Add(vertIndex);

					meshTriangles.Add(meshVertexArray.Add(vertex));
					if (z < 50) {
						vertColors.Add(FColor::Black);
					}
					else {
						vertColors.Add(FColor::Green);
					}

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

						uint8 cubeIndex = marchCubes[x][y][z].GetCubeIndex(surfaceLevel);

						//Loop through all Vertecies in a Cube
						for (int32 i = triTable[cubeIndex].Num() - 1; i >= 0; --i) {
							uint8 edgeIndex = triTable[cubeIndex][i];

							FVector relEdgeCenter = relativEdgeCenters[edgeIndex];


							FVector vertex = BlockToWorldLocation(FVector(x - borderSize, y - borderSize, z) + relEdgeCenter) + FVector(50, 50, 50);//(FVector(x, y, z) + relEdgeCenter) * blockSize + FVector(50, 50, 50);

							uint32 vertIndex = vertexArray.Add(vertex);
							triangles.Add(vertIndex);
							marchCubes[x][y][z].holdingVertexIndecies.Add(vertIndex);

							if (meshVertexArray.Contains(vertex)) {
								meshVertexArray.Add(vertex);

								vertColors.Add(FColor::White);
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

	CalculateNormalsAndTangents();

	FOccluderVertexArray meshNormals;
	TArray<FProcMeshTangent> meshTangents;

	for (size_t i = 0; i < vertexArray.Num(); i++) {

		if (!borderVertexIndecies.Contains(i)) {
			meshNormals.Add(normals[i]);
			meshTangents.Add(tangents[i]);
		}

	}

	CreateMeshSection(0, meshVertexArray, meshTriangles, meshNormals, TArray<FVector2D>(), vertColors, meshTangents, true);
	*/
}


void UProceduralMeshGeneratorComponent::CreateMarchCubes(const TArray<TArray<TArray<FBlockInfo>>>& blockGrid){

	for (size_t x = 0; x < gridSize.X - 1; x++) {
		marchCubes.Add(TArray<TArray<FMarchCube>>());
		for (size_t y = 0; y < gridSize.Y - 1; y++) {
			marchCubes[x].Add(TArray<FMarchCube>());
			for (size_t z = 0; z < gridSize.Z - 1; z++) {
				marchCubes[x][y].Add(FMarchCube());

				marchCubes[x][y][z].corners.Add(blockGrid[x + 1][y][z].value);
				marchCubes[x][y][z].corners.Add(blockGrid[x + 1][y + 1][z].value);
				marchCubes[x][y][z].corners.Add(blockGrid[x][y + 1][z].value);
				marchCubes[x][y][z].corners.Add(blockGrid[x][y][z].value);
				marchCubes[x][y][z].corners.Add(blockGrid[x + 1][y][z + 1].value);
				marchCubes[x][y][z].corners.Add(blockGrid[x + 1][y + 1][z + 1].value);
				marchCubes[x][y][z].corners.Add(blockGrid[x][y + 1][z + 1].value);
				marchCubes[x][y][z].corners.Add(blockGrid[x][y][z + 1].value);

				marchCubes[x][y][z].cornerColors.Add(blockGrid[x + 1][y][z].color);
				marchCubes[x][y][z].cornerColors.Add(blockGrid[x + 1][y + 1][z].color);
				marchCubes[x][y][z].cornerColors.Add(blockGrid[x][y + 1][z].color);
				marchCubes[x][y][z].cornerColors.Add(blockGrid[x][y][z].color);
				marchCubes[x][y][z].cornerColors.Add(blockGrid[x + 1][y][z + 1].color);
				marchCubes[x][y][z].cornerColors.Add(blockGrid[x + 1][y + 1][z + 1].color);
				marchCubes[x][y][z].cornerColors.Add(blockGrid[x][y + 1][z + 1].color);
				marchCubes[x][y][z].cornerColors.Add(blockGrid[x][y][z + 1].color);

			}
		}
	}

}

void UProceduralMeshGeneratorComponent::MarchingCubes(bool bBorderNormalsOnly){

	//TArray<FVector> uniqueVertex;
	vertexArray.Empty();
	triangles.Empty();
	meshVertexArray.Empty();
	meshTriangles.Empty();

	normals.Empty();
	tangents.Empty();
	vertColors.Empty();
	borderVertexIndecies.Empty();
	 

	int32 borderSize = bBorderNormalsOnly ? 1 : 0;
	FVector cubeGridSize = FVector( gridSize.X - 1, gridSize.Y - 1, gridSize.Z - 1);

	//loop through cubeGrid without border
	for (size_t x = borderSize; x < cubeGridSize.X - borderSize; x++) {
		for (size_t y = borderSize; y < cubeGridSize.Y - borderSize; y++) {


			
			for (size_t z = 0; z < cubeGridSize.Z; z++) {
				uint8 cubeIndex = marchCubes[x][y][z].GetCubeIndex(surfaceLevel);
				
				if (z == 0) {
					//DrawDebugBox(GetWorld(), BlockToWorldLocation(FVector(x, y, z) - borderSize) + FVector(50,50,50), FVector(50, 50, 50), FColor::Purple, false, 60, 0, 1);
				}
				 
				//Loop through all Vertecies in a Cube 
				for (int32 i = triTable[cubeIndex].Num()-1; i >= 0; --i) {
					uint8 edgeIndex = triTable[cubeIndex][i];

					FVector relEdgeCenter = relativEdgeCenters[edgeIndex];


					FVector vertex = BlockToWorldLocation(FVector(x - borderSize, y - borderSize, z) + relEdgeCenter) + FVector(50, 50, 50);// (FVector(x, y, z) + relEdgeCenter) * blockSize + FVector(50, 50, 50);

						//if (bDrawDebug) {
							if (z > 40 && z < 50) {
								//DrawDebugBox(GetWorld(), vertex, FVector(2, 2, 2), FColor::Purple, false, 60, 0, 1); 
							}
							
						//}  

						int32 vertIndex = vertexArray.Add(vertex);
						triangles.Add(vertIndex);
						marchCubes[x][y][z].holdingVertexIndecies.Add(vertIndex);
						 
						meshTriangles.Add(meshVertexArray.Add(vertex));
						if (z < 50) {
							vertColors.Add(FColor::Black);
						}
						else {
							vertColors.Add(FColor::Green);
						}

					
				}

				if (bDrawDebug) {
					//DrawDebugBox(GetWorld(), FVector(x, y, z)* blockSize, FVector(100, 100, 100), FColor::White, false, 30, 0, 1);
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

						uint8 cubeIndex = marchCubes[x][y][z].GetCubeIndex(surfaceLevel);

						//Loop through all Vertecies in a Cube
						for (int32 i = triTable[cubeIndex].Num() - 1; i >= 0; --i) {
							uint8 edgeIndex = triTable[cubeIndex][i];

							FVector relEdgeCenter = relativEdgeCenters[edgeIndex];

							FVector vertex = BlockToWorldLocation(FVector(x - borderSize, y - borderSize, z)  + relEdgeCenter) + FVector(50, 50, 50);//(FVector(x, y, z) + relEdgeCenter) * blockSize + FVector(50, 50, 50);

							uint32 vertIndex = vertexArray.Add(vertex);
							triangles.Add(vertIndex);
							marchCubes[x][y][z].holdingVertexIndecies.Add(vertIndex);

							if (meshVertexArray.Contains(vertex)) {
								meshVertexArray.Add(vertex);
								
								vertColors.Add(FColor::White);
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

	

	//UE_LOG(LogTemp, Warning, TEXT("Marching Cubes: triangles:%i, Vertecies:%i"), triangles.Num(), vertexArray.Num());

	//UKismetProceduralMeshLibrary::CalculateTangentsForMesh(vertexArray, triangles, UVs, normals, tangents);
	CalculateNormalsAndTangents();

	FOccluderVertexArray meshNormals;
	TArray<FProcMeshTangent> meshTangents;

	for (size_t i = 0; i < vertexArray.Num(); i++){ 
		 
		if (!borderVertexIndecies.Contains(i)) { 
			meshNormals.Add(normals[i]);  
			meshTangents.Add(tangents[i]); 
			 
			
			//Draw Normals
			//DrawDebugLine(GetWorld(), GetComponentLocation() + vertexArray[i], GetComponentLocation() + normals[i] * 100 + vertexArray[i], FColor::Red, false, 60, 0, 1);
		}

	}

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *GetComponentLocation().ToString());

	//UE_LOG(LogTemp, Warning, TEXT("meshVertecies:%i, meshVertColors:%i"), meshVertexArray.Num(), vertColors.Num()); 

	
	 
	CreateMeshSection(0, meshVertexArray, meshTriangles, meshNormals, TArray<FVector2D>(), vertColors, meshTangents, true);

	
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
 


void UProceduralMeshGeneratorComponent::CalculateNormalsAndTangents(){

	float normalsMultiplyer = 0.0f;
	
	if (vertexArray.Num() == 0)
	{ 
		return;
	}

	// Number of triangles 
	const int32 NumTris = triangles.Num() / 3; 
	// Number of verts
	const int32 NumVerts = vertexArray.Num();

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
			int32 VertIndex = FMath::Min(triangles[(TriIdx * 3) + CornerIdx], NumVerts - 1);

			CornerIndex[CornerIdx] = VertIndex;
			P[CornerIdx] = vertexArray[VertIndex];

			// Find/add this vert to index buffer
			TArray<int32> VertOverlaps;
			FindVertOverlaps(VertIndex, vertexArray, VertOverlaps);

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
	for (int VertxIdx = 0; VertxIdx < vertexArray.Num(); VertxIdx++)
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

	normals.Reset();
	normals.AddUninitialized(NumVerts);

	tangents.Reset();
	tangents.AddUninitialized(NumVerts);

	int32 innerIndex = 0;
	for (int VertxIdx = 0; VertxIdx < NumVerts; VertxIdx++)
	{
		
		//if (!borderVertexIndecies.Contains(VertxIdx)) {


			FVector& TangentX = VertexTangentXSum[VertxIdx];
			FVector& TangentY = VertexTangentYSum[VertxIdx];
			FVector& TangentZ = VertexTangentZSum[VertxIdx];

			TangentX.Normalize();
			TangentZ.Normalize();

			normals[VertxIdx] = TangentZ;

			// Use Gram-Schmidt orthogonalization to make sure X is orth with Z
			TangentX -= TangentZ * (TangentZ | TangentX);
			TangentX.Normalize();
			 
			// See if we need to flip TangentY when generating from cross product
			const bool bFlipBitangent = ((TangentZ ^ TangentX) | TangentY) < 0.f;

			tangents[VertxIdx] = FProcMeshTangent(TangentX, bFlipBitangent);

			innerIndex++;
		//}
	}
}


