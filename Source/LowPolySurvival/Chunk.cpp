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
#include "ChunkColumn.h"


 
// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComponent;

	proceduralMesh = CreateDefaultSubobject<UProceduralMeshGeneratorComponent>("Procedural Mesh");
	proceduralMesh->SetupAttachment(RootComponent);
	proceduralMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
	proceduralMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
	proceduralMesh->SetRelativeLocation(FVector(100, 100, 100));

}

// Called when the game starts or when spawned 
void AChunk::BeginPlay(){
	Super::BeginPlay();
	
	gameInstance = GetGameInstance<UMyGameInstance>();
}

void AChunk::Init(AWorldGenerator * _worldGenerator, AChunkColumn* _chunkColumn){
	worldGenerator = _worldGenerator;
	chunkColumn = _chunkColumn;
}

void AChunk::InitBlockGrid(){

	uint8 dim = FWorldParams::chunkSize;
	gridDim = FIntVector(dim, dim, dim);

	blockGrid.Init(TArray<TArray<FBlockData>>(), gridDim.X); 
	for (size_t x = 0; x < gridDim.X; x++) {
		blockGrid[x].Init(TArray<FBlockData>(), gridDim.Y);

		for (size_t y = 0; y < gridDim.Y; y++) {
			blockGrid[x][y].Init(FBlockData(), gridDim.Z);
		}
	}

	ApplyNoiseOnGrid();


}

void AChunk::TopDownTrace(){

	
	FCollisionQueryParams params;
	params.bReturnPhysicalMaterial = true;
	 
	TArray<FHitResult> hitResults;
	FVector start = GetActorLocation() + FVector(0, 0, FWorldParams::buildHeight);
	FVector end = GetActorLocation() + FVector(0, 0, FWorldParams::deathZone);
	GetWorld()->LineTraceMultiByChannel(hitResults, start, end, ECollisionChannel::ECC_WorldStatic, params); 

	if (bDrawDebug) {
		//DrawDebugLine(GetWorld(), start, end, FColor::Green, true, 100, 0, 5);
		float halfChunkWorldSize = (FWorldParams::blockSize*FWorldParams::chunkSize) / 2;
		FVector center = FVector(halfChunkWorldSize);
		DrawDebugBox(GetWorld(), GetActorLocation() + center, center, FColor::Green, true, 100, 0, 5);
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
					blockGrid[x][y][z].blockId = 1;
				}
			}
		}
	}
}

void AChunk::ApplyNoiseOnGrid(){

	auto terrainNoiseMap = *chunkColumn->GetTerrainNoiseMap();

	int32 terrainBlockLevelInChunk = FWorldParams::terrainBlockLevel - (chunkLoc.Z * FWorldParams::chunkSize);

	FIntVector blockLoc = ChunkToBlockLocation(chunkLoc);

	for (int32 x = 0; x < gridDim.X; x++) {
		for (int32 y = 0; y < gridDim.Y; y++) {

			int32 upAmount = terrainNoiseMap[x][y] * FWorldParams::terrainNoiseHeight;
			int32 maxZ = FWorldParams::terrainBlockLevel + upAmount;
			int32 maxZInChunk = (terrainBlockLevelInChunk + upAmount) > FWorldParams::chunkSize ? FWorldParams::chunkSize : terrainBlockLevelInChunk + upAmount;

			for (int32 z = 0; z < maxZInChunk; z++) {

				int32 blockZ = blockLoc.Z + z;

				if (blockZ == 0) {
					blockGrid[x][y][z].blockId = 1;
				}else if (blockZ < (maxZ - 2)) {
					blockGrid[x][y][z].blockId = 2;
				}
				else{
					blockGrid[x][y][z].blockId = 3;
				}
				
			}


		}
	}
	 
	AddNoiseCaves();
}

void AChunk::AddNoiseCaves(){

	FIntVector blockLoc = ChunkToBlockLocation(chunkLoc);
	float caveNoiseLevel = 0.2f;

	for (int32 x = 0; x < gridDim.X; x++) {
		for (int32 y = 0; y < gridDim.Y; y++) {
			for (int32 z = 0; z < gridDim.Z; z++) {

				float noise = worldGenerator->CaveNoise(FVector(blockLoc.X + x, blockLoc.Y + y, blockLoc.Z + z));

				if (noise < caveNoiseLevel) { 
					blockGrid[x][y][z].blockId = 0;
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

void AChunk::Create(FIntVector _chunkLoc){
	chunkLoc = _chunkLoc;

	InitBlockGrid();

	TopDownTrace();
	
}

void AChunk::Load(FIntVector _chunkLoc){
	chunkLoc = _chunkLoc;

	//TODO remove this
	Create(_chunkLoc);
}

void AChunk::Unload(){
	Destroy();
}

void AChunk::GenerateTerrainMesh(){

	if (!bIsTerrainGenerated) {
		proceduralMesh->GenerateMesh(this);
		bIsTerrainGenerated = true;
	}
	
}

void AChunk::UpdateTerrainMesh(const FIntVector &chunkBlockLoc){
	proceduralMesh->UpdateMesh(this, chunkBlockLoc);
}

void AChunk::SetBlock(FIntVector gridLoc, const FBlockData &blockData){
	if (gridLoc.X > -1 && gridLoc.Y > -1 && gridLoc.Z > -1 && gridLoc.X <= gridDim.X && gridLoc.Y <= gridDim.Y && gridLoc.Z <= gridDim.Z) {

		blockGrid[gridLoc.X][gridLoc.Y][gridLoc.Z] = blockData;

	}

	proceduralMesh->UpdateMesh(this, gridLoc);
}


void AChunk::SetTerrainMaterial(UMaterialInterface * material){
	proceduralMesh->SetMaterial(0, material);
}

const FBlockData& AChunk::GetBlock(const FIntVector &chunkBlockLoc) const
{
	return blockGrid[chunkBlockLoc.X][chunkBlockLoc.Y][chunkBlockLoc.Z];
}

const TArray<TArray<TArray<FBlockData>>>* AChunk::GetGridData() const{
	return &blockGrid;
}

const AWorldGenerator * AChunk::GetWorldGenerator() const{
	return worldGenerator;
}

FIntVector AChunk::GetChunkLocation() const{
	return chunkLoc;
}

TArray<TArray<FBlockData>>& AChunk::operator[](int32 index){
	return blockGrid[index];
}

const TArray<TArray<FBlockData>>& AChunk::operator[](int32 index) const{
	return blockGrid[index];
}

FBlockData& AChunk::operator[](const FIntVector & chunkBlockLoc){
	return blockGrid[chunkBlockLoc.X][chunkBlockLoc.Y][chunkBlockLoc.Z];
}

const FBlockData& AChunk::operator[](const FIntVector & chunkBlockLoc) const{
	return blockGrid[chunkBlockLoc.X][chunkBlockLoc.Y][chunkBlockLoc.Z];
}

