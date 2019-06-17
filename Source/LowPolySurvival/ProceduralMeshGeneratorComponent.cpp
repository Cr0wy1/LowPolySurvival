// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralMeshGeneratorComponent.h"
#include "DrawDebugHelpers.h"
#include "SimplexNoise.h"




UProceduralMeshGeneratorComponent::UProceduralMeshGeneratorComponent(const FObjectInitializer & ObjectInitializer) : UProceduralMeshComponent(ObjectInitializer){

	UE_LOG(LogTemp, Warning, TEXT("Procedural Mesh Generator Contrsuct"));


}

void UProceduralMeshGeneratorComponent::GenerateMesh(){
	FOccluderVertexArray vertexArray;
	vertexArray.Add(FVector(0, 0, 0));
	vertexArray.Add(FVector(100, 0, 0));
	vertexArray.Add(FVector(0, 100, 0));

	TArray<int32> triangles = { 2, 1, 0 };
	FlushPersistentDebugLines(GetWorld());
	CreateCornerGrid();
	CreateMarchCubes();
	MarchingCubes();

	

	//CreateMeshSection(x*sizeY + y, vertexArray, triangles, FOccluderVertexArray(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);

}

void UProceduralMeshGeneratorComponent::CreateCornerGrid(){
	
	 
	for (size_t x = 0; x < gridSize.X; x++) {
		grid.Add(TArray<TArray<float>>());
		for (size_t y = 0; y < gridSize.Y; y++) {
			grid[x].Add(TArray<float >());

			float noiseValue = (USimplexNoise::SimplexNoise2D((float)x*offset, (float)y*offset) + 1) / 2;

			

			int32 clamped = FMath::FloorToInt(noiseValue*gridSize.Z);

			UE_LOG(LogTemp, Warning, TEXT("CreateCornerGrid: clamped: %i"), clamped);

			for (size_t z = 0; z < gridSize.Z; z++) {

				grid[x][y].Add(0);
				
			}
			grid[x][y][clamped] = 1;
			DrawDebugBox(GetWorld(), FVector(x, y, clamped)*blockSize, FVector(3, 3, 3), FColor::Green, false, 30, 0, 1);

			


			//for (size_t z = 0; z < clamped; z++) {
				//grid[x][y][z] = 1;

				//if (bDrawDebug) {

					//DrawDebugBox(GetWorld(), FVector(x, y, z)*blockSize, FVector(3, 3, 3), FColor::Green, false, 30, 0, 1);
					//if (noiseValue > surfaceLevel) {
						
					//}
					//else {
						//DrawDebugBox(GetWorld(), FVector(x, y, z)*blockSize, FVector(3, 3, 3), FLinearColor(noiseValue, noiseValue, noiseValue).ToFColor(true), false, 30, 0, 1);
					//}
				//}
			//}

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

void UProceduralMeshGeneratorComponent::MarchingCubes(){

	FOccluderVertexArray vertexArray;
	
	TArray<int32> triangles;

	for (size_t x = 0; x < gridSize.X - 1; x++) {
		for (size_t y = 0; y < gridSize.Y - 1; y++) {
			for (size_t z = 0; z < gridSize.Z - 1; z++) {
				uint8 cubeIndex = marchCubes[x][y][z].GetCubeIndex(surfaceLevel);
				
				//UE_LOG(LogTemp, Warning, TEXT("MarchingCubes: cubeIndex: %i"), cubeIndex);

				TArray<uint8> uniqueVerticies;

				for (int32 i = triTable[cubeIndex].Num()-1; i >= 0; --i) {
					uint8 edgeIndex = triTable[cubeIndex][i];
					int32 triIndex = 0;
					//if (uniqueVerticies.Find(edgeIndex, triIndex)) {
						//triangles.Add(triIndex);
					//}
					//else {
						FVector vertex = (FVector(x, y, z) + relativEdgeCenters[edgeIndex]) * blockSize + FVector(50, 50, 50);

						if (bDrawDebug) {
							DrawDebugBox(GetWorld(), vertex, FVector(2, 2, 2), FColor::Purple, false, 30, 0, 1);
						} 
						

						
						triangles.Add(vertexArray.Add(vertex));
						
						//UE_LOG(LogTemp, Warning, TEXT("Marching Cubes: TrianglePoint: %i"), triangles.Last());

					//}
					
				}

				FVector cubeGridSize = gridSize - 1;

				//UE_LOG(LogTemp, Warning, TEXT("Marching Cubes: %i"), triangles.Num());

			}
		}
	}



	CreateMeshSection(0, vertexArray, triangles, FOccluderVertexArray(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);


}
