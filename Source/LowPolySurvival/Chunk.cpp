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
	//proceduralMesh->AddRelativeLocation(FVector(-100, -100, -100));
	proceduralMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
	proceduralMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);

}

// Called when the game starts or when spawned 
void AChunk::BeginPlay(){
	Super::BeginPlay();
	
	gameInstance = GetGameInstance<UMyGameInstance>();
	worldInfo = gameInstance->GetWorldInfo();
}

void AChunk::InitBlockGrid(){

	gridDim = FVector(worldInfo->chunkSize / worldInfo->blockSize + 3, worldInfo->chunkSize / worldInfo->blockSize + 3, 200);

	blockGrid.Init(TArray<TArray<FBlockInfo>>(), gridDim.X);
	for (size_t x = 0; x < gridDim.X; x++) {
		blockGrid[x].Init(TArray<FBlockInfo>(), gridDim.Y);

		for (size_t y = 0; y < gridDim.Y; y++) {
			blockGrid[x][y].Init(FBlockInfo(), gridDim.Z);
		}
	}

	ApplyNoiseOnGrid();
	//RandomizeGrid(100, 5);


	if (bDrawDebug) {  
		for (size_t x = 0; x < blockGrid.Num(); x++){
			for (size_t y = 0; y < blockGrid[0].Num(); y++) {
				for (size_t z = 0; z < blockGrid[0][0].Num(); z++) {
					//FVector blockLoc = GetActorLocation() + (FVector(x, y, z) * worldInfo->blockSize) + FVector(worldInfo->blockSize / 2);
					//DrawDebugPoint(GetWorld(), blockLoc, 5, FColor::Red, false, 30);
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
		//DrawDebugLine(GetWorld(), start, end, FColor::Green, true, 100, 0, 5);
		DrawDebugBox(GetWorld(), GetActorLocation() + FVector(500,500, 10000), FVector(500, 500, worldInfo->buildHeight / 2), FColor::Green, true, 100, 0, 5);
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

	float frequency = 3.0f;
	uint8 octaves = 6;

	int32 islandOffset = 50;
	int32 upOffset = 10;
	int32 downOffset = 30;

	

	for (size_t x = 0; x < blockGrid.Num(); x++) {
		for (size_t y = 0; y < blockGrid[0].Num(); y++) {
			float noise = 0.0f;

			FVector blockLoc = ChunkToBlockLocation(chunkLoc);
			//UE_LOG(LogTemp, Warning, TEXT("%s"), *blockLoc.ToString()); 

			float nx = (blockLoc.X + x) * 0.01f - 0.5f;
			float ny = (blockLoc.Y + y) * 0.01f - 0.5f;

			//with frequency
			//float noise = USimplexNoise::SimplexNoise2D((blockLoc.X +x) * frequency, (blockLoc.Y + y) * frequency);

			//with Octaves
			float persistence = 0.5f;
			float maxValue = 0;
			float amplitude = 128.0f;
			float cOctaveFrequency = 1.0f;
			for (size_t i = 0; i < octaves; i++) {
				noise += USimplexNoise::SimplexNoise2D(nx * cOctaveFrequency, ny * cOctaveFrequency) * amplitude;
				UE_LOG(LogTemp, Warning, TEXT("noise: %f"), noise);
				maxValue += amplitude;
				amplitude *= persistence;
				
				cOctaveFrequency *= 2;
			}
			noise /= maxValue;
			
			//Apply Redistribution
			float redistribution = 2.0f;
			noise = FMath::Pow(noise, redistribution);

			//Apply Terraces
			//uint32 terracesSteps = 2;
			//noise = FMath::RoundToFloat(noise * terracesSteps) / terracesSteps;

			

			uint32 downAmount = noise * downOffset;
			uint32 upAmount = noise * upOffset;

			//if (noise > 0.1) {

				blockGrid[x][y][islandOffset - downAmount].value = noise;
				blockGrid[x][y][islandOffset + upAmount].value = noise;
				 
				for (size_t z = islandOffset - downAmount + 1; z < islandOffset; z++) {
					blockGrid[x][y][z].value = 1;
				}  

				for (size_t z = islandOffset + upAmount - 1; z >= islandOffset; z--) {
					blockGrid[x][y][z].value = 1;
				}
				

			//}

		}
	}

}

void AChunk::ApplyNoise3DOnGrid(){

	float noiseScale = 0.01f;
	int32 islandOffset = 50;
	int32 upOffset = 10;
	int32 downOffset = 30;

	FVector blockLoc = ChunkToBlockLocation(chunkLoc);

	for (size_t x = 0; x < blockGrid.Num(); x++) {
		for (size_t y = 0; y < blockGrid[0].Num(); y++) {
			for (size_t z = 0; z < blockGrid[0][0].Num(); z++) {

				float noise = USimplexNoise::SimplexNoise3D((blockLoc.X + x) * noiseScale, (blockLoc.Y + y) * noiseScale, (blockLoc.Z + z) * noiseScale);

				blockGrid[x][y][z].value = -50 + ((noise + 1) / 2);

				//int32 downAmount = noise * downOffset + (noise * downOffset * 0.6);
				//int32 upAmount = noise * upOffset;

				//if (noise > 0.1) {

				//	blockGrid[x][y][islandOffset - downAmount].value = 1;
				//	blockGrid[x][y][islandOffset + upAmount].value = 1;

				//	for (size_t z = islandOffset - downAmount; z < islandOffset; z++) {
				//		blockGrid[x][y][z].value = 1;
				//	}

				//	for (size_t z = islandOffset + upAmount; z >= islandOffset; z--) {
				//		blockGrid[x][y][z].value = 1;
				//	}



				//	int32 iOuter = islandOffset - downAmount;

				//	float outerNoise = USimplexNoise::SimplexNoise3D((blockLoc.X + x) * 0.1f, (blockLoc.Y + y)*0.1f, iOuter*0.1f);

				//	if (outerNoise > 0.1) {
				//		blockGrid[x][y][iOuter].value = 0;
				//	}


				//}
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

	InitBlockGrid();

	////Spawn Island
	//float baseSpawnChance = 0.2f;
	//UDataTable* islandTable = GetGameInstance<UMyGameInstance>()->GetIslandTable();

	//TArray<FIslandInfo*> islandInfos;
	//islandTable->GetAllRows(FString(), islandInfos);

	//float spawnChance = (FMath::Rand() % 100) * 0.01;
	//
	//if (spawnChance <= baseSpawnChance) {
	//	float zLoc = ((FMath::Rand() % 1000) - 500) * 100;
	//	float zRot = FMath::Rand() % 360;

	//	//GetWorld()->SpawnActor<AIsland>(islandInfos[0]->island_BP, FVector(chunkLoc * chunkSize, zLoc), FRotator(0, zRot, 0));
	//}

	TopDownTrace();
	
}

void AChunk::Load(FVector2D _chunkLoc){
	chunkLoc = _chunkLoc;
}

void AChunk::Unload(){
	Destroy();
}

void AChunk::RemoveBlock(int32 gridX, int32 gridY, int32 gridZ){
	gridX += 1;
	gridY += 1;
	if (gridX > -1 && gridY > -1 && gridZ > -1 && gridX <= gridDim.X && gridY <= gridDim.Y && gridZ <= gridDim.Z) {
		//UE_LOG(LogTemp, Warning, TEXT("removeBlock"));

		blockGrid[gridX][gridY][gridZ].value = 0;

	}

	proceduralMesh->UpdateMesh(blockGrid, FIntVector(gridX,gridY,gridZ));
}

void AChunk::RemoveBlock(FIntVector gridLoc){
	gridLoc.X += 1;
	gridLoc.Y += 1;
	if (gridLoc.X > -1 && gridLoc.Y > -1 && gridLoc.Z > -1 && gridLoc.X <= gridDim.X && gridLoc.Y <= gridDim.Y && gridLoc.Z <= gridDim.Z) {
		//UE_LOG(LogTemp, Warning, TEXT("removeBlock"));

		blockGrid[gridLoc.X][gridLoc.Y][gridLoc.Z].value = 0;

	}


	proceduralMesh->UpdateMesh(blockGrid, gridLoc);
}

void AChunk::SetTerrainMaterial(UMaterialInterface * material){
	proceduralMesh->SetMaterial(0, material);
}

