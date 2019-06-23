// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralMeshGeneratorComponent.h"
#include "DrawDebugHelpers.h"
#include "SimplexNoise.h"
#include "Chunk.h"
#include "KismetProceduralMeshLibrary.h"


UProceduralMeshGeneratorComponent::UProceduralMeshGeneratorComponent(const FObjectInitializer & ObjectInitializer) : UProceduralMeshComponent(ObjectInitializer){


}

void UProceduralMeshGeneratorComponent::GenerateMesh(){


	FlushPersistentDebugLines(GetWorld());
	CreateCornerGrid();
	CreateMarchCubes();
	MarchingCubes(false);

}

void UProceduralMeshGeneratorComponent::GenerateMesh(const TArray<TArray<TArray<FBlockInfo>>>& blockGrid){

	gridSize = FVector(blockGrid.Num(), blockGrid.IsValidIndex(0) ? blockGrid[0].Num() : 0, blockGrid[0].IsValidIndex(0) ? blockGrid[0][0].Num() : 0);

	CreateMarchCubes(blockGrid);
	MarchingCubes();
}

void UProceduralMeshGeneratorComponent::CreateCornerGrid(){
	
	 
	for (size_t x = 0; x < gridSize.X; x++) {
		grid.Add(TArray<TArray<float>>());
		for (size_t y = 0; y < gridSize.Y; y++) {
			grid[x].Add(TArray<float >());

			float noiseValue = (USimplexNoise::SimplexNoise2D((float)x*0.1, (float)y*0.1) + 1) / 2;

			int32 clamped = FMath::FloorToInt(noiseValue*offset*gridSize.Z);

			//UE_LOG(LogTemp, Warning, TEXT("CreateCornerGrid: clamped: %i"), clamped);

			for (size_t z = 0; z < gridSize.Z; z++) {

				grid[x][y].Add(0);
				
			}

			for (size_t z = 0; z < clamped; z++) {

				grid[x][y][z] = 1;


				
			}

			grid[x][y][clamped] = noiseValue * 0.5 * lerpMultiply;
			if (clamped > 0) {
				grid[x][y][clamped - 1] = noiseValue * lerpMultiply;
			}

			if (clamped < grid[x][y].Num() - 1) {
				grid[x][y][clamped + 1] = noiseValue * 0.25 * lerpMultiply;
			}
			

			UE_LOG(LogTemp, Warning, TEXT("noiseValue: %f"), noiseValue);


			if (bDrawDebug) {
				DrawDebugBox(GetWorld(), FVector(x, y, clamped)*blockSize, FVector(3, 3, 3), FLinearColor(noiseValue, noiseValue, noiseValue, 1).ToFColor(true), false, 30, 0, 1);
			}

		}
		
	}
}

void UProceduralMeshGeneratorComponent::CreateMarchCubes(){

	for (size_t x = 0; x < gridSize.X - 1; x++) {
		marchCubes.Add(TArray<TArray<FMarchCube>>());
		for (size_t y = 0; y < gridSize.Y - 1; y++) {
			marchCubes[x].Add(TArray<FMarchCube>());
			for (size_t z = 0; z < gridSize.Z - 1; z++) {
				marchCubes[x][y].Add(FMarchCube());

				marchCubes[x][y][z].corners.Add(grid[x+1][y][z]);
				marchCubes[x][y][z].corners.Add(grid[x+1][y+1][z]);
				marchCubes[x][y][z].corners.Add(grid[x][y+1][z]);
				marchCubes[x][y][z].corners.Add(grid[x][y][z]);
				marchCubes[x][y][z].corners.Add(grid[x+1][y][z+1]);
				marchCubes[x][y][z].corners.Add(grid[x+1][y+1][z+1]);
				marchCubes[x][y][z].corners.Add(grid[x][y+1][z+1]);
				marchCubes[x][y][z].corners.Add(grid[x][y][z+1]);
			
			}
		}
	}

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

	int32 borderSize = bBorderNormalsOnly ? 1 : 0;
	FVector cubeGridSize = FVector( gridSize.X - (1 + borderSize), gridSize.Y - (1 + borderSize), gridSize.Z - 1);

	//loop through cubeGrid without border
	for (size_t x = borderSize; x < cubeGridSize.X; x++) { 
		for (size_t y = borderSize; y < cubeGridSize.Y; y++) {
			for (size_t z = 0; z < cubeGridSize.Z; z++) {
				uint8 cubeIndex = marchCubes[x][y][z].GetCubeIndex(surfaceLevel);
				
				//Loop through all Vertecies in a Cube
				for (int32 i = triTable[cubeIndex].Num()-1; i >= 0; --i) {
					uint8 edgeIndex = triTable[cubeIndex][i];

					FVector relEdgeCenter = relativEdgeCenters[edgeIndex];

					if (bUseLerp) {

						uint8 edgeCor1 = edgeCorners[edgeIndex][0];
						uint8 edgeCor2 = edgeCorners[edgeIndex][1];

						float edgeCorValue1 = marchCubes[x][y][z].corners[edgeCor1];
						float edgeCorValue2 = marchCubes[x][y][z].corners[edgeCor2];

						if (relEdgeCenter.X == 0) {
							relEdgeCenter.X = (edgeCorValue2 - edgeCorValue1) / 2;
						}
						else if (relEdgeCenter.Y == 0) {
							relEdgeCenter.Y = (edgeCorValue2 - edgeCorValue1) / 2;
						}
						else {
							relEdgeCenter.Z = (edgeCorValue2 - edgeCorValue1) / 2;
						}
					}

					FVector vertex = (FVector(x, y, z) + relEdgeCenter) * blockSize + FVector(50, 50, 50);
					int32 vertIndex = 0;
					//if (uniqueVertex.Find(vertex, vertIndex)) {
						//triangles.Add(vertIndex);
					//}
					//else {
						
						if (bDrawDebug) {
							DrawDebugBox(GetWorld(), vertex, FVector(2, 2, 2), FColor::Purple, false, 30, 0, 1);
						} 
						
						triangles.Add(vertexArray.Add(vertex));
						meshTriangles.Add(meshVertexArray.Add(vertex));

						//uniqueVertex.Add(vertex);

						//if (cubeIndex < 127) {
							UVs.Add(FVector2D(200.0f / 1024, 170.0f / 1024));
							meshUV.Add(FVector2D(200.0f / 1024, 170.0f / 1024));
						//}
						//else {
							//uvs.Add(FVector2D(vertex.X * 0.0001, vertex.Y*0.0001));
						//}
					//}
					
				}

				if (bDrawDebug) {
					DrawDebugBox(GetWorld(), FVector(x, y, z)* blockSize, FVector(100, 100, 100), FColor::White, false, 30, 0, 1);
				}
			}
		}
	}

	//loop through cubeGride Border
	if (bBorderNormalsOnly) {
		for (size_t x = 0; x < cubeGridSize.X + 1; x++) {
			for (size_t y = 0; y < cubeGridSize.Y + 1; y++) {

				//check if index is border
				if (x == 0 || y == 0 || x == (cubeGridSize.X) || y == (cubeGridSize.Y)) {
					//UE_LOG(LogTemp, Warning, TEXT("loop: x:%i, y:%i"), x, y);

					for (size_t z = 0; z < cubeGridSize.Z; z++) {

						uint8 cubeIndex = marchCubes[x][y][z].GetCubeIndex(surfaceLevel);

						//Loop through all Vertecies in a Cube
						for (int32 i = triTable[cubeIndex].Num() - 1; i >= 0; --i) {
							uint8 edgeIndex = triTable[cubeIndex][i];

							FVector relEdgeCenter = relativEdgeCenters[edgeIndex];

							if (bUseLerp) {

								uint8 edgeCor1 = edgeCorners[edgeIndex][0];
								uint8 edgeCor2 = edgeCorners[edgeIndex][1];

								float edgeCorValue1 = marchCubes[x][y][z].corners[edgeCor1];
								float edgeCorValue2 = marchCubes[x][y][z].corners[edgeCor2];

								if (relEdgeCenter.X == 0) {
									relEdgeCenter.X = (edgeCorValue2 - edgeCorValue1) / 2;
								}
								else if (relEdgeCenter.Y == 0) {
									relEdgeCenter.Y = (edgeCorValue2 - edgeCorValue1) / 2;
								}
								else {
									relEdgeCenter.Z = (edgeCorValue2 - edgeCorValue1) / 2;
								}
							}

							FVector vertex = (FVector(x, y, z) + relEdgeCenter) * blockSize + FVector(50, 50, 50);
							//int32 vertIndex = 0;
							//if (uniqueVertex.Find(vertex, vertIndex)) {
								//borderTriangles.Add(vertIndex);
							//}
							//else {

							int32 vertIndex = vertexArray.Add(vertex);
							triangles.Add(vertIndex);

							if (meshVertexArray.Contains(vertex)) {
								meshVertexArray.Add(vertex);
								meshUV.Add(FVector2D(200.0f / 1024, 170.0f / 1024));
							}
							else {
								borderVertexIndecies.Add(vertIndex);
							}
								//vertexArray.Add(vertex);
								//UVs.Add(FVector2D(200.0f / 1024, 170.0f / 1024));

								

								//uniqueVertex.Add(vertex);
								//borderUVs.Add(FVector2D(200.0f / 1024, 170.0f / 1024));
							//}

						}
					}
				}


			}
		}
	}

	

	UE_LOG(LogTemp, Warning, TEXT("Marching Cubes: triangles:%i, Vertecies:%i"), triangles.Num(), vertexArray.Num());

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

	
	CreateMeshSection(0, meshVertexArray, meshTriangles, meshNormals, meshUV, TArray<FColor>(), meshTangents, true);

	
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
	for (int VertxIdx = 0; VertxIdx < vertexArray.Num(); VertxIdx++){

		int32 TriIdx = *VertToTriMap.Find(VertxIdx);
		VertexTangentZSum[VertxIdx] = FaceTangentZ[TriIdx]; 

		// Find relevant triangles for tangents
		TArray<int32> TangentTris;
		VertToTriMap.MultiFind(VertxIdx, TangentTris);

		for (int i = 0; i < TangentTris.Num(); i++)
		{
			int32 TriIdx = TangentTris[i];
			VertexTangentXSum[VertxIdx] = FaceTangentX[TriIdx];  
			VertexTangentYSum[VertxIdx] = FaceTangentY[TriIdx];
		}
	}

	// Finally, normalize tangents and build output arrays

	normals.Reset();
	normals.AddUninitialized(NumVerts);

	tangents.Reset();
	tangents.AddUninitialized(NumVerts);

	for (int VertxIdx = 0; VertxIdx < NumVerts; VertxIdx++)
	{

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

	}
}


