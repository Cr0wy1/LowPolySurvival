// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "Island.h"
#include "MyGameInstance.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Engine/DataTable.h"
#include "WorldGenerator.h"
#include "Buildings.h"
#include "ProceduralMeshGeneratorComponent.h"
#include "SimplexNoise.h"



// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComponent;

	proceduralMesh = CreateDefaultSubobject<UProceduralMeshGeneratorComponent>("Procedural Mesh");
	proceduralMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AChunk::BeginPlay(){
	Super::BeginPlay();
	
	gameInstance = GetGameInstance<UMyGameInstance>();
	worldInfo = gameInstance->GetWorldInfo();
}

void AChunk::InitBlockGrid(){

	blockGrid.Init(TArray<TArray<FBlockInfo>>(), worldInfo->chunkSize / worldInfo->blockSize +1);
	for (size_t x = 0; x < blockGrid.Num(); x++) {
		blockGrid[x].Init(TArray<FBlockInfo>(), worldInfo->chunkSize / worldInfo->blockSize +1);

		for (size_t y = 0; y < blockGrid[0].Num(); y++) {
			blockGrid[x][y].Init(FBlockInfo(), 200);
		}
	}

	ApplyNoiseOnGrid();
	//RandomizeGrid(100, 5);


	if (bDrawDebug) {  
		for (size_t x = 0; x < blockGrid.Num(); x++){
			for (size_t y = 0; y < blockGrid[0].Num(); y++) {
				for (size_t z = 0; z < blockGrid[0][0].Num(); z++) {
					FVector blockLoc = GetActorLocation() + (FVector(x, y, z) * worldInfo->blockSize) + FVector(worldInfo->blockSize / 2);
					DrawDebugPoint(GetWorld(), blockLoc, 5, FColor::Red, false, 30);
				}
			}
		}
	}
	
	proceduralMesh->GenerateMesh(blockGrid);
}

void AChunk::TopDownTrace(){

	
	FCollisionQueryParams params;
	params.bReturnPhysicalMaterial = true;

	TArray<FHitResult> hitResults;
	FVector start = GetActorLocation() + FVector(0, 0, worldInfo->buildHeight);
	FVector end = GetActorLocation() + FVector(0, 0, worldInfo->deathZone);
	GetWorld()->LineTraceMultiByChannel(hitResults, start, end, ECollisionChannel::ECC_WorldStatic, params);

	if (bDrawDebug) {
		DrawDebugLine(GetWorld(), start, end, FColor::Green, true, 100, 0, 5);
	}
	

	UDataTable* worldGenTable = gameInstance->GetWorldGenTable();
	
	TArray<FWorldGenInfo*> worldGenInfos;
	worldGenTable->GetAllRows(FString(), worldGenInfos);

	for (FHitResult &hiteResult : hitResults) {

		if (hiteResult.Actor.IsValid()) {
			UE_LOG(LogTemp, Warning, TEXT("TopDownTrace: Actor: %s"), *(*hiteResult.Actor).GetName());
		}

		if (hiteResult.PhysMaterial.IsValid()) {
			UPhysicalMaterial* physicMaterial = hiteResult.PhysMaterial.Get();

			if (physicMaterial->SurfaceType == PM_GRASS) {

				UE_LOG(LogTemp, Warning, TEXT("TopDownTrace: physic Material: %s"), *physicMaterial->GetName());

				for (FWorldGenInfo* worldGenInfo : worldGenInfos) {
					if (worldGenInfo->biome == EBiome::GRASS) {
						ABuildings* building = GetWorld()->SpawnActor<ABuildings>(worldGenInfo->Plant_BP, hiteResult.ImpactPoint, FRotator::ZeroRotator);
						
					}
				}
			}

			
			
		}
	}

}

void AChunk::RandomizeGrid(int32 zLine, int32 blockAmount){
	for (size_t x = 0; x < blockGrid.Num(); x++) {
		for (size_t y = 0; y < blockGrid[0].Num(); y++) {
			int32 randZ = FMath::Rand() % blockAmount;
			for (size_t z = 0; z < blockGrid[0][0].Num(); z++) {
				if (z < (zLine + randZ)) {
					blockGrid[x][y][z].value = 1;
				}
			}
		}
	}
}

void AChunk::ApplyNoiseOnGrid(){

	

	for (size_t x = 0; x < blockGrid.Num(); x++) {
		for (size_t y = 0; y < blockGrid[0].Num(); y++) {
			float noise = USimplexNoise::SimplexNoise2D(chunkLoc.X + x*0.1, chunkLoc.Y + y*0.1);
			int32 blockAmount = 10 * noise;
			for (size_t z = 0; z < blockGrid[0][0].Num(); z++) {
				if (z < (100 + blockAmount)) {
					blockGrid[x][y][z].value = 1;
				}
			}
		}
	}

}

// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChunk::Create(FVector2D _chunkLoc){
	chunkLoc = _chunkLoc;
	
	float chunkSize = worldInfo->chunkSize;

	float baseSpawnChance = 0.2f;

	InitBlockGrid();

	UDataTable* islandTable = GetGameInstance<UMyGameInstance>()->GetIslandTable();

	TArray<FIslandInfo*> islandInfos;
	islandTable->GetAllRows(FString(), islandInfos);

	float spawnChance = (FMath::Rand() % 100) * 0.01;
	
	if (spawnChance <= baseSpawnChance) {
		float zLoc = ((FMath::Rand() % 1000) - 500) * 100;
		float zRot = FMath::Rand() % 360;

		GetWorld()->SpawnActor<AIsland>(islandInfos[0]->island_BP, FVector(chunkLoc * chunkSize, zLoc), FRotator(0, zRot, 0));
	}

	TopDownTrace();
	
}

void AChunk::Load(FVector2D _chunkLoc){
	chunkLoc = _chunkLoc;
}

void AChunk::Unload(){
	Destroy();
}

