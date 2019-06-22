// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralMeshGeneratorComponent.h"
#include "DrawDebugHelpers.h"
#include "SimplexNoise.h"
#include "Chunk.h"


UProceduralMeshGeneratorComponent::UProceduralMeshGeneratorComponent(const FObjectInitializer & ObjectInitializer) : UProceduralMeshComponent(ObjectInitializer){


}

void UProceduralMeshGeneratorComponent::GenerateMesh(){


	FlushPersistentDebugLines(GetWorld());
	CreateCornerGrid();
	CreateMarchCubes();
	MarchingCubes();

}

void UProceduralMeshGeneratorComponent::GenerateMesh(const TArray<TArray<TArray<FBlockInfo>>>& blockGrid){

	gridSize = FVector(blockGrid.Num(), blockGrid.IsValidIndex(0) ? blockGrid[0].Num() : 0, blockGrid[0].IsValidIndex(0) ? blockGrid[0][0].Num() : 0);

	UE_LOG(LogTemp, Warning, TEXT("Generate Mesh"));

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

void UProceduralMeshGeneratorComponent::MarchingCubes(){

	FOccluderVertexArray vertexArray;
	TArray<FVector> uniqueVertex;
	TArray<int32> triangles;

	for (size_t x = 0; x < gridSize.X - 1; x++) {
		for (size_t y = 0; y < gridSize.Y - 1; y++) {
			for (size_t z = 0; z < gridSize.Z - 1; z++) {
				uint8 cubeIndex = marchCubes[x][y][z].GetCubeIndex(surfaceLevel);
				

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


					//UE_LOG(LogTemp, Warning, TEXT("relEdgeCenter: %s"), *relEdgeCenter.ToString());


					FVector vertex = (FVector(x, y, z) + relEdgeCenter) * blockSize + FVector(50, 50, 50);
					int32 vertIndex = 0;
					if (uniqueVertex.Find(vertex, vertIndex)) {
						triangles.Add(vertIndex);
					}
					else {
						

						if (bDrawDebug) {
							DrawDebugBox(GetWorld(), vertex, FVector(2, 2, 2), FColor::Purple, false, 30, 0, 1);
						} 
						

						
						triangles.Add(vertexArray.Add(vertex));
						uniqueVertex.Add(vertex);
						//UE_LOG(LogTemp, Warning, TEXT("Marching Cubes: TrianglePoint: %i"), triangles.Last());

					}
					
				}

				if (bDrawDebug) {
					DrawDebugBox(GetWorld(), FVector(x, y, z)* blockSize, FVector(100, 100, 100), FColor::White, false, 30, 0, 1);
				}
				

				

			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Marching Cubes: triangles:%i, Vertecies:%i"), triangles.Num(), vertexArray.Num());

	CreateMeshSection(0, vertexArray, triangles, FOccluderVertexArray(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);


}
