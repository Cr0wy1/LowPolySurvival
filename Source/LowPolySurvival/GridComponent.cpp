// Fill out your copyright notice in the Description page of Project Settings.


#include "GridComponent.h"
#include "Chunk.h"
#include "SimplexNoise.h"
#include "ProceduralMeshGeneratorComponent.h"
// Sets default values for this component's properties
UGridComponent::UGridComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	//InitGrid();
}


// Called when the game starts
void UGridComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGridComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGridComponent::InitGrid(){
	grid.Init(TArray<TArray<FBlockData>>(), gridSize.X);
	for (size_t x = 0; x < gridSize.X; x++) {
		grid[x].Init(TArray<FBlockData>(), gridSize.Y);
		for (size_t y = 0; y < gridSize.Y; y++) {
			grid[x][y].Init(FBlockData(), gridSize.Z);
		}
	}

}

void UGridComponent::RandomizeGrid() {
	for (size_t x = 0; x < gridSize.X; x++) { 
		for (size_t y = 0; y < gridSize.Y; y++) {
			float rand = (float)(FMath::Rand() % 100) / 100.0f;
			for (size_t z = 0; z < gridSize.Z; z++) {
				grid[x][y][z].value = rand;

			}
		}
	}
}

void UGridComponent::ApplySimplexNoise2D(){
	for (size_t x = 0; x < gridSize.X; x++) {
		for (size_t y = 0; y < gridSize.Y; y++) {
			
			float noise = Noise(x, y);

			uint16 noiseHeight = noise * gridSize.Z;

			for (size_t z = 0; z < noiseHeight; z++) {
				grid[x][y][z].value = noise;

			}
		}
	}
}

void UGridComponent::ApplySimplexNoise3D(){

	FVector mid = gridSize / 2;

	for (size_t x = 0; x < gridSize.X; x++) {
		for (size_t y = 0; y < gridSize.Y; y++) {
			for (size_t z = 0; z < gridSize.Z; z++) {

			float distance = (mid - FVector(x, y, z)).Size();

			float noise = Noise3D(x, y, z, noiseParams)  * ((float)z / mid.Z) * (1-distance/mid.X);

			grid[x][y][z].value = noise;

			}
		}
	}

}

//NOT FINISHED
void UGridComponent::ApplyDiamondSquare(){

	uint16 squareSize = gridSize.X;

	while (squareSize > 0) {

		for (size_t x = 0; x < gridSize.X; x+=squareSize) {
			for (size_t y = 0; y < gridSize.Y; y+=squareSize) {
			
				uint16 cornerX = x + (squareSize - 1);
				uint16 cornerY = y + (squareSize - 1);

				uint16 midX = x + (squareSize / 2.0f)-1;
				uint16 midY = y + (squareSize / 2.0f)-1;

				float cornersValue = grid[x][y][0].value + grid[cornerX][y][0].value + grid[x][cornerY][0].value + grid[cornerX][cornerY][0].value;

				
			
			}
		}

	}
}

const TArray<TArray<TArray<FBlockData>>>* UGridComponent::GetGridPointer() const
{
	return &grid;
}

