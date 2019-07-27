// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "Island.h"
#include "MyGameInstance.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Engine/DataTable.h"
#include "WorldGenerator.h"
#include "Buildings.h"
#include "ProceduralMeshGeneratorComponent.h"
#include "ChunkColumn.h"
#include "LowPolySurvivalCharacter.h"
#include "Item.h"

 
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

	proceduralMesh->onGeneratedMesh.AddDynamic(this, &AChunk::OnGeneratedMesh);
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

	blockGrid.Init(TArray<TArray<FBlock>>(), gridDim.X);
	for (size_t x = 0; x < gridDim.X; x++) {
		blockGrid[x].Init(TArray<FBlock>(), gridDim.Y);

		for (size_t y = 0; y < gridDim.Y; y++) {
			blockGrid[x][y].Init(FBlock(), gridDim.Z);
		}
	}

	ApplyNoiseOnGrid();
	AddNoiseCaves();
	AddNoiseOres();

}

void AChunk::TopDownTrace(const FIntVector &blockLoc){

	FVector worldLoc = BlockToWorldLocation(FVector(blockLoc));
	worldLoc.X += FMath::FRand() * 100.0f - 50.0f;
	worldLoc.Y += FMath::FRand() * 100.0f - 50.0f;

	FHitResult hitResult;
	FVector start = worldLoc + FVector(0, 0, 100);
	FVector end = worldLoc + FVector(0, 0, -100);
	GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_WorldStatic);

	if (bDrawDebug) {
		DrawDebugLine(GetWorld(), start, end, FColor::Green, true, 100, 0, 5);
		float halfChunkWorldSize = (FWorldParams::blockSize*FWorldParams::chunkSize) / 2;
		FVector center = FVector(halfChunkWorldSize);
		//DrawDebugBox(GetWorld(), GetActorLocation() + center, center, FColor::Green, true, 100, 0, 5);
	} 
	

	UDataTable* worldGenTable = gameInstance->GetWorldGenTable();
	 
	TArray<FWorldGenInfo*> worldGenInfos;
	worldGenTable->GetAllRows(FString(), worldGenInfos);

	if (hitResult.Actor.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("TopDownTrace: Actor: %s"), *(*hitResult.Actor).GetName());


		for (FWorldGenInfo* worldGenInfo : worldGenInfos) {
			if (worldGenInfo->biome == EBiome::GRASS && worldGenInfo->type == EVegetationType::TREE) {
				ABuildings* building = GetWorld()->SpawnActor<ABuildings>(worldGenInfo->Plant_BP, hitResult.ImpactPoint, FRotator::ZeroRotator);

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
					blockGrid[x][y][z].data.blockId = 1;
				}
			}
		}
	}
}

void AChunk::ApplyNoiseOnGrid(){

	UDataTable* resourceTable = gameInstance->GetResourceTable();
	FResource* dirtR = resourceTable->FindRow<FResource>("1", FString());
	FResource* grassR = resourceTable->FindRow<FResource>("2", FString());
	FResource* stoneR = resourceTable->FindRow<FResource>("3", FString());
	FResource* earthcoreR = resourceTable->FindRow<FResource>("4", FString());

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
					blockGrid[x][y][z].SetResource(earthcoreR);

				}else if (blockZ < (maxZ - 5)) {
					blockGrid[x][y][z].SetResource(stoneR);

				}else if (blockZ < (maxZ - 2)) {
					blockGrid[x][y][z].SetResource(dirtR);

				}else{
					blockGrid[x][y][z].SetResource(grassR);

				}
				
			}
		}
	}
	 
	
}

void AChunk::AddNoiseCaves(){

	FIntVector blockLoc = ChunkToBlockLocation(chunkLoc);
	float caveNoiseLevel = 0.2f;

	//Check if is earthcore level to ensure not digging holes at this
	int32 zInit = chunkLoc.Z < 1 ? 1 : 0;
	
	for (int32 x = 0; x < gridDim.X; x++) {
		for (int32 y = 0; y < gridDim.Y; y++) {
			for (int32 z = zInit; z < gridDim.Z; z++) {

				float noise = worldGenerator->CaveNoise(FVector(blockLoc.X + x, blockLoc.Y + y, blockLoc.Z + z));

				if (noise < caveNoiseLevel) { 
					blockGrid[x][y][z].data.blockId = 0;
				}
			}

		} 
	}
}

void AChunk::AddNoiseOres(){

	UDataTable* resourceTable = gameInstance->GetResourceTable();
	FResource* coalR = resourceTable->FindRow<FResource>("5", FString());
	FResource* ironR = resourceTable->FindRow<FResource>("6", FString());
	FResource* goldR = resourceTable->FindRow<FResource>("7", FString());

	FIntVector blockLoc = ChunkToBlockLocation(chunkLoc);
	float oreNoiseLevel = 0.06f;

	for (int32 x = 0; x < gridDim.X; x++) {
		for (int32 y = 0; y < gridDim.Y; y++) {
			for (int32 z = 0; z < gridDim.Z; z++) {

				float noiseCoal = worldGenerator->OreNoise(FVector(blockLoc.X + x, blockLoc.Y + y, blockLoc.Z + z));
				float noiseIron = worldGenerator->OreNoise(FVector(blockLoc.X + x * 2, blockLoc.Y + y * 2, blockLoc.Z + z * 2));
				float noiseGold = worldGenerator->OreNoise(FVector(blockLoc.X + x * 3, blockLoc.Y + y * 3, blockLoc.Z + z * 3));


				if (noiseCoal < 0.08f) {
					blockGrid[x][y][z].SetResource(coalR);
				}
				if (noiseIron < 0.04) {
					blockGrid[x][y][z].SetResource(ironR);
				}
				if (noiseGold < 0.02) {
					blockGrid[x][y][z].SetResource(goldR);
				}
			}

		}
	}
}

void AChunk::OnGeneratedMesh(){
	//UE_LOG(LogTemp, Warning, TEXT("OnGeneratedMesh"));

	auto terrainNoiseMap = *chunkColumn->GetTerrainNoiseMap();

	FIntVector blockLoc = ChunkToBlockLocation(chunkLoc);

	int32 terrainBlockLevelInChunk = FWorldParams::terrainBlockLevel - (chunkLoc.Z * FWorldParams::chunkSize);

	for (int32 x = 0; x < gridDim.X; x++) {
		for (int32 y = 0; y < gridDim.Y; y++) {
		
			int32 upAmount = terrainNoiseMap[x][y] * FWorldParams::terrainNoiseHeight;
			int32 maxZInChunk = (terrainBlockLevelInChunk + upAmount) > FWorldParams::chunkSize ? FWorldParams::chunkSize : terrainBlockLevelInChunk + upAmount;

			
			//Plant Vegetetion
			if (maxZInChunk < FWorldParams::chunkSize) {
				float rand = FMath::FRand();
				if (rand > 0.9) {
					TopDownTrace(FIntVector(blockLoc.X + x, blockLoc.Y + y, blockLoc.Z + maxZInChunk));
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

void AChunk::HitBlock(FIntVector gridLoc, float damageAmount, AActor* causer){
	if (gridLoc.X > -1 && gridLoc.Y > -1 && gridLoc.Z > -1 && gridLoc.X <= gridDim.X && gridLoc.Y <= gridDim.Y && gridLoc.Z <= gridDim.Z) {

		if (blockGrid[gridLoc.X][gridLoc.Y][gridLoc.Z].ApplyDamage(damageAmount)) {
			ALowPolySurvivalCharacter* player = Cast<ALowPolySurvivalCharacter>(causer);
			if (player) {
				FItemStack itemStack = FItemStack::FromId(this, 20, 5);
				player->AddItemStackToInventory(itemStack, true);
			}
			SetBlockUnsafe(gridLoc, FBlockData());
		}

	}
}

void AChunk::SetBlock(FIntVector gridLoc, const FBlockData &blockData){
	if (gridLoc.X > -1 && gridLoc.Y > -1 && gridLoc.Z > -1 && gridLoc.X <= gridDim.X && gridLoc.Y <= gridDim.Y && gridLoc.Z <= gridDim.Z) {

		blockGrid[gridLoc.X][gridLoc.Y][gridLoc.Z].data = blockData;

	}

	proceduralMesh->UpdateMesh(this, gridLoc);
}

void AChunk::SetBlockUnsafe(FIntVector gridLoc, const FBlockData & blockData){
	blockGrid[gridLoc.X][gridLoc.Y][gridLoc.Z].data = blockData;

	proceduralMesh->UpdateMesh(this, gridLoc);
}


void AChunk::SetTerrainMaterial(UMaterialInterface * material){
	proceduralMesh->SetMaterial(0, material);
}

const FBlock& AChunk::GetBlock(const FIntVector &chunkBlockLoc) const
{
	return blockGrid[chunkBlockLoc.X][chunkBlockLoc.Y][chunkBlockLoc.Z];
}

const FBlock & AChunk::GetBlock(const FVector & worldLoc) const
{
	FIntVector blockLoc = WorldToBlockLocation(worldLoc);
	FIntVector chunkBlockLoc;
	BlockToChunkBlockLocation(blockLoc, chunkBlockLoc);
	
	return GetBlock(chunkBlockLoc);
}

const TArray<TArray<TArray<FBlock>>>* AChunk::GetGridData() const{
	return &blockGrid;
}

const AWorldGenerator * AChunk::GetWorldGenerator() const{
	return worldGenerator;
}

FIntVector AChunk::GetChunkLocation() const{
	return chunkLoc;
}

TArray<TArray<FBlock>>& AChunk::operator[](int32 index){
	return blockGrid[index];
}

const TArray<TArray<FBlock>>& AChunk::operator[](int32 index) const{
	return blockGrid[index];
}

FBlock& AChunk::operator[](const FIntVector & chunkBlockLoc){
	return blockGrid[chunkBlockLoc.X][chunkBlockLoc.Y][chunkBlockLoc.Z];
}

const FBlock& AChunk::operator[](const FIntVector & chunkBlockLoc) const{
	return blockGrid[chunkBlockLoc.X][chunkBlockLoc.Y][chunkBlockLoc.Z];
}

