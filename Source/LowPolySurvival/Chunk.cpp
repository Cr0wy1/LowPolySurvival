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
#include "SaveRegion.h"
#include "Kismet/GameplayStatics.h"
#include "Async.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstance.h"

 
// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComponent;

	proceduralMesh = CreateDefaultSubobject<UProceduralMeshGeneratorComponent>("Procedural Mesh");
	proceduralMesh->SetupAttachment(RootComponent);
	proceduralMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
	proceduralMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
	
	proceduralMesh->onGeneratedMesh.AddDynamic(this, &AChunk::OnGeneratedMesh);
	
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> materialClassFinder(TEXT("MaterialInstance'/Game/Terrain/TerrainColor_MI.TerrainColor_MI'"));
	SetTerrainMaterial(materialClassFinder.Object);
} 

// Called when the game starts or when spawned 
void AChunk::BeginPlay(){
	Super::BeginPlay();
	
	gameInstance = GetGameInstance<UMyGameInstance>();
	
}

void AChunk::Init(AWorldGenerator * _worldGenerator, UChunkColumn* _chunkColumn){
	worldGenerator = _worldGenerator;
	chunkColumn = _chunkColumn;
}


// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UE_LOG(LogTemp, Warning, TEXT("ticking"));

}

AChunk * AChunk::Construct(AWorldGenerator * worldGenerator, FIntVector chunkLoc){

	FIntVector chunkColLoc = chunkLoc;
	chunkColLoc.Z = 0;
	auto findChunkCol = FWorldLoader::loadedChunkColumns.Find(chunkColLoc);

	UChunkColumn* responseChunkColumn = findChunkCol ? *findChunkCol : UChunkColumn::Construct(worldGenerator, chunkColLoc);


	AChunk* newChunk = worldGenerator->GetWorld()->SpawnActor<AChunk>(AChunk::StaticClass(), ChunkToWorldLocation(chunkLoc), FRotator::ZeroRotator);
	newChunk->Init(worldGenerator, responseChunkColumn);

	FWorldLoader::loadedChunks.Add(chunkLoc, newChunk);


	newChunk->Load(chunkLoc);

	return newChunk;
}

void AChunk::Create() {
	
	InitBlockGrid();

	ApplyNoiseOnGrid();
	AddNoiseCaves();
	AddNoiseOres(); 
	//AddWater();

	bNeedsSaving = true;
} 

void AChunk::Load(FIntVector _chunkLoc) {
	chunkLoc = _chunkLoc;

	if (!LoadFromFile()) {
		Create();
	}
	bIsTaskFinished = true;
	//(new FAutoDeleteAsyncTask<ChunkTask>(this, false))->StartBackgroundTask();
}

void AChunk::Unload() {

	//(new FAutoDeleteAsyncTask<ChunkTask>(this, true))->StartBackgroundTask();

	proceduralMesh->EnsureCompletion();

	for (AActor* actor : loadedActors) {
		actor->Destroy();
	} 

	Destroy();
}

void AChunk::GenerateTerrainMesh() {

	if (!bIsTerrainGenerated && bIsTaskFinished) {
		proceduralMesh->GenerateMesh(this); 
		bIsTerrainGenerated = true;
	}
	else {
		bShouldGenerateMesh = true;
	}
}

void AChunk::InitBlockGrid(){

	uint8 dim = FWorldParams::chunkSize;
	gridDim = FIntVector(dim, dim, dim);

	blockGrid.Init(dim, dim, dim);
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
		//UE_LOG(LogTemp, Warning, TEXT("TopDownTrace: Actor: %s"), *(*hitResult.Actor).GetName());


		for (FWorldGenInfo* worldGenInfo : worldGenInfos) {
			if (worldGenInfo->biome == EBiome::GRASS && worldGenInfo->type == EVegetationType::TREE) {
				ABuildings* building = GetWorld()->SpawnActor<ABuildings>(worldGenInfo->Plant_BP, hitResult.ImpactPoint, FRotator::ZeroRotator);
				loadedActors.Add(building);
			}
		}
	}

}

void AChunk::RandomizeGrid(int32 zLine, int32 blockAmount){
	for (size_t x = 0; x < blockGrid.dims.X; x++) {
		for (size_t y = 0; y < blockGrid.dims.Y; y++) {
			int32 randZ = FMath::Rand() % blockAmount;
			for (size_t z = 0; z < blockGrid.dims.Z; z++) {
				if (z < (zLine + randZ)) {
					blockGrid[x][y][z].data.bIsSolid = true;
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
	FResource* sandR = resourceTable->FindRow<FResource>("9", FString());
	FResource* snowR = resourceTable->FindRow<FResource>("10", FString());
	
	FBiomeData* cBiome;
	FResource* biomeR = grassR;
	

	auto terrainNoiseMap = *chunkColumn->GetTerrainNoiseMap();
	auto heatNoiseMap = *chunkColumn->GetHeatNoiseMap();
	auto rainNoiseMap = *chunkColumn->GetRainNoiseMap();
	auto hillsNoiseMap = *chunkColumn->GetHillsNoiseMap();
	

	int32 terrainBlockLevelInChunk = FWorldParams::terrainBlockLevel - (chunkLoc.Z * FWorldParams::chunkSize);

	FIntVector blockLoc = ChunkToBlockLocation(chunkLoc);

	for (int32 x = 0; x < gridDim.X; x++) {
		for (int32 y = 0; y < gridDim.Y; y++) {

			cBiome = worldGenerator->GetBiome(heatNoiseMap[x][y], rainNoiseMap[x][y]);

			//float biomeNoise = worldGenerator->BiomeNoise(FVector2D(blockLoc.X + x, blockLoc.Y + y));
			biomeR = cBiome->baseBlockResource;

			int32 upAmount = terrainNoiseMap[x][y] * 100;
			//if (hillsNoiseMap[x][y] > 0.2) {
				//upAmount += hillsNoiseMap[x][y] * 100;
			//}
			// FWorldParams::terrainNoiseHeight;
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
					blockGrid[x][y][z].SetResource(biomeR);
					if (cBiome->overrideResourceColor) {
						blockGrid[x][y][z].SetBiomeColor(cBiome->biomeColor);
					}
					
				}
				
			}
		}
	}
	 
	
}

void AChunk::AddNoiseCaves(){

	FResource* airR = FResource::FromId(this, 0);

	FIntVector blockLoc = ChunkToBlockLocation(chunkLoc);
	float caveNoiseLevel = 0.3f;
	 
	//Check if is earthcore level to ensure not digging holes at this
	int32 zInit = chunkLoc.Z < 1 ? 1 : 0;
	
	for (int32 x = 0; x < gridDim.X; x++) {
		for (int32 y = 0; y < gridDim.Y; y++) {
			for (int32 z = zInit; z < gridDim.Z; z++) {

				float noise = worldGenerator->CaveNoise(FVector(blockLoc.X + x, blockLoc.Y + y, blockLoc.Z + z));

				if (noise < caveNoiseLevel) { 
					blockGrid[x][y][z].SetResource(airR);
					
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

				if (blockGrid[x][y][z].resource && blockGrid[x][y][z].resource->id == 3) {

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
}

void AChunk::AddWater(){
	UDataTable* resourceTable = gameInstance->GetResourceTable();
	FResource* waterR = resourceTable->FindRow<FResource>("8", FString());

	FIntVector blockLoc = ChunkToBlockLocation(chunkLoc);

	for (int32 x = 0; x < gridDim.X; x++) {
		for (int32 y = 0; y < gridDim.Y; y++) {
			for (int32 z = 0; z < gridDim.Z; z++) {

				if (blockLoc.Z + z <= 20) {
					blockGrid[x][y][z].SetResource(waterR);
				}
				

			}

		}
	}
}

void AChunk::OnGeneratedMesh(){
	//UE_LOG(LogTemp, Warning, TEXT("OnGeneratedMesh"));

	if (!bIsFullCreated) {

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
						//TopDownTrace(FIntVector(blockLoc.X + x, blockLoc.Y + y, blockLoc.Z + maxZInChunk));
					}

				}
			}
		}
	}

	bIsFullCreated = true;

	if (bNeedsFluidUpdate) {
		FluidUpdate();
		UpdateTerrainMesh(chunkLoc);
	}
}

void AChunk::FluidUpdate(){
	bNeedsFluidUpdate = false;
	FBlock* block = nullptr;
	AChunk* targetChunk = nullptr;
	for (int32 x = 0; x < gridDim.X; x++) {
		for (int32 y = 0; y < gridDim.Y; y++) {
			for (int32 z = 0; z < gridDim.Z; z++) {
				
				if (blockGrid[x][y][z].resource && blockGrid[x][y][z].resource->type == EResourceType::FLUID) {
					block = GetBlockIncludeNearby(FIntVector(x, y, z - 1));
					if (block && block->IsAir()) {
						block->SetResource(blockGrid[x][y][z].resource);
						blockGrid[x][y][z].SetResource(FResource::FromId(this, 0));
						bNeedsFluidUpdate = true;
					}
					//if (z > 0 && blockGrid[x][y][z - 1].IsAir() ) {
					//	blockGrid[x][y][z - 1].SetResource(blockGrid[x][y][z].resource);
					//	blockGrid[x][y][z].SetResource(FResource::FromId(this, 0));
					//	bNeedsFluidUpdate = true;
					//}
					else {

						TArray<FBlock*> blocksToSplitOn;
						TArray<AChunk*> targetChunks;

						block = GetBlockIncludeNearby(FIntVector(x-1, y, z), targetChunk);
						targetChunks.Add(targetChunk);
						if (block && block->IsAir()) {
							blocksToSplitOn.Add(block);
						}

						block = GetBlockIncludeNearby(FIntVector(x + 1, y, z), targetChunk);
						targetChunks.Add(targetChunk);
						if (block && block->IsAir()) {
							blocksToSplitOn.Add(block);
						}

						block = GetBlockIncludeNearby(FIntVector(x, y-1, z), targetChunk);
						targetChunks.Add(targetChunk);
						if (block && block->IsAir()) {
							blocksToSplitOn.Add(block);
						}

						block = GetBlockIncludeNearby(FIntVector(x, y+1, z), targetChunk);
						targetChunks.Add(targetChunk);
						if (block && block->IsAir()) {
							blocksToSplitOn.Add(block);
						}

						if (blocksToSplitOn.Num() > 0 ) {

							int32 splitDurability = blockGrid[x][y][z].durability / blocksToSplitOn.Num();

							if (splitDurability > 0) {
								blockGrid[x][y][z].durability = splitDurability;

								for (FBlock* block : blocksToSplitOn) {
									block->SetResource(blockGrid[x][y][z].resource);
									block->durability = splitDurability;
								}

								for (AChunk* chunk : targetChunks) {
									if (chunk) {
										chunk->bNeedsFluidUpdate = true;
										chunk->UpdateTerrainMesh(FIntVector(0, 0, 0));
									}
								
								}

								//blockGrid[x][y][z].SetResource(FResource::FromId(this, 0));

								bNeedsFluidUpdate = true;
							}

						}

					}

				}

			}
		}
	}
}

void AChunk::SaveToFile(){
	if (bNeedsSaving) {
		USaveRegion* saveRegion = Cast<USaveRegion>(UGameplayStatics::CreateSaveGameObject(USaveRegion::StaticClass()));
		saveRegion->SetGrid(blockGrid);
		FString slotName = worldGenerator->GetWorldName() + "/chunks/" + FString::FromInt(chunkLoc.X) + "." + FString::FromInt(chunkLoc.Y) + "." + FString::FromInt(chunkLoc.Z);
		UGameplayStatics::SaveGameToSlot(saveRegion, slotName, 0);

		bNeedsSaving = false;
	}
}

bool AChunk::LoadFromFile(){
	FString slotName = worldGenerator->GetWorldName() + "/chunks/" + FString::FromInt(chunkLoc.X) + "." + FString::FromInt(chunkLoc.Y) + "." + FString::FromInt(chunkLoc.Z);
	
	if (UGameplayStatics::DoesSaveGameExist(slotName, 0)) {
		USaveRegion* saveRegion = Cast<USaveRegion>(UGameplayStatics::LoadGameFromSlot(slotName, 0));
		if (saveRegion) {
			saveRegion->GetGrid(this, blockGrid);

			gridDim = blockGrid.dims;

			return true;
		}
	}

	return false;
}

void AChunk::OnEndTask(AChunk* chunk){

	chunk->bIsTaskFinished = true;

	//if (chunk && chunk->bShouldGenerateMesh) {

		chunk->GenerateTerrainMesh();
	//}
}




void AChunk::UpdateTerrainMesh(const FIntVector &chunkBlockLoc){
	proceduralMesh->UpdateMesh(this, chunkBlockLoc);
} 

void AChunk::HitBlock(FIntVector gridLoc, float damageAmount, AActor* causer){
	if (gridLoc.X > -1 && gridLoc.Y > -1 && gridLoc.Z > -1 && gridLoc.X <= gridDim.X && gridLoc.Y <= gridDim.Y && gridLoc.Z <= gridDim.Z) {

		FBlock* hittedBlock = &blockGrid[gridLoc.X][gridLoc.Y][gridLoc.Z]; 

		UE_LOG(LogTemp, Warning, TEXT("Chunk hittedBlock: %s"), *hittedBlock->ToString());


		if (hittedBlock->ApplyDamage(damageAmount)) {
			ALowPolySurvivalCharacter* player = Cast<ALowPolySurvivalCharacter>(causer);
			if (player && hittedBlock->resource) {
				FItemStack itemStack = FItemStack::FromId(this, 20, hittedBlock->resource->id, 5);
				player->AddItemStackToInventory(itemStack, true);
			}
			SetBlockUnsafe(gridLoc, FBlock::FromId(this, 0));
		}

	}
}

bool AChunk::SetBlock(FIntVector gridLoc, const FBlock& block){
	if (gridLoc.X > -1 && gridLoc.Y > -1 && gridLoc.Z > -1 && gridLoc.X <= gridDim.X && gridLoc.Y <= gridDim.Y && gridLoc.Z <= gridDim.Z) {

		blockGrid[gridLoc.X][gridLoc.Y][gridLoc.Z] = block;
		SetBlockUnsafe(gridLoc, block);

		return true;
	}

	return false;
}

void AChunk::SetBlockUnsafe(FIntVector gridLoc, const FBlock& block){
	blockGrid[gridLoc.X][gridLoc.Y][gridLoc.Z] = block;
	FluidUpdate();
	bNeedsSaving = true;
	proceduralMesh->UpdateMesh(this, gridLoc);
}


void AChunk::SetTerrainMaterial(UMaterialInterface * material){
	proceduralMesh->SetMaterial(0, material);
}

const FBlock* AChunk::GetBlock(const FIntVector &chunkBlockLoc) const
{
	return &blockGrid[chunkBlockLoc.X][chunkBlockLoc.Y][chunkBlockLoc.Z];
}

const FBlock* AChunk::GetBlock(const FVector & worldLoc) const
{
	FIntVector blockLoc = WorldToBlockLocation(worldLoc);
	FIntVector chunkBlockLoc;
	BlockToChunkBlockLocation(blockLoc, chunkBlockLoc);
	
	return GetBlock(chunkBlockLoc);
}

FBlock* AChunk::GetBlockIncludeNearby(FIntVector chunkBlockLoc, AChunk* OUT_targetChunk) const{

	FIntVector relativeChunk = FIntVector(0, 0, 0);

	if (chunkBlockLoc.Z < 0) {
		chunkBlockLoc.Z = FWorldParams::chunkSize - 1;
		relativeChunk.Z = -1;
	}else if(chunkBlockLoc.Z > FWorldParams::chunkSize-1){
		chunkBlockLoc.Z = 0;
		relativeChunk.Z = 1;
	}

	if (chunkBlockLoc.X < 0) {
		chunkBlockLoc.X = FWorldParams::chunkSize - 1;
		relativeChunk.X = -1;
	}else if (chunkBlockLoc.X > FWorldParams::chunkSize - 1) {
		chunkBlockLoc.X = 0;
		relativeChunk.X = 1;
	}

	if (chunkBlockLoc.Y < 0) {
		chunkBlockLoc.Y = FWorldParams::chunkSize - 1;
		relativeChunk.Y = -1;
	}else if (chunkBlockLoc.Y > FWorldParams::chunkSize - 1) {
		chunkBlockLoc.Y = 0;
		relativeChunk.Y = 1;
	}

	OUT_targetChunk = worldGenerator->GetChunkSafe(chunkLoc + relativeChunk);
	if (OUT_targetChunk) {
		
		return &(*OUT_targetChunk)[chunkBlockLoc];
	}

	return nullptr;
}

const FBlockGrid* AChunk::GetGridData() const{
	return &blockGrid;
}

AWorldGenerator * AChunk::GetWorldGenerator() const{
	return worldGenerator;
}

FIntVector AChunk::GetChunkLocation() const{
	return chunkLoc;
}

AChunk * AChunk::GetNearbyChunk(EGridDir gridDir) const{

	switch (gridDir){
		case EGridDir::FRONT:
			return worldGenerator->GetChunkSafe(chunkLoc + FGridDir::FRONT);
		case EGridDir::RIGHT:
			return worldGenerator->GetChunkSafe(chunkLoc + FGridDir::RIGHT);
		case EGridDir::UP:
			return worldGenerator->GetChunkSafe(chunkLoc + FGridDir::UP);
		case EGridDir::BACK:
			return worldGenerator->GetChunkSafe(chunkLoc + FGridDir::BACK);
		case EGridDir::LEFT:
			return worldGenerator->GetChunkSafe(chunkLoc + FGridDir::LEFT);
		case EGridDir::DOWN:
			return worldGenerator->GetChunkSafe(chunkLoc + FGridDir::DOWN);
	}

	return nullptr;
}

TArray<AChunk*> AChunk::GetNearbyChunks(TArray<EGridDir> gridDirs) const{

	TArray<AChunk*> nearbyChunks;

	size_t gridDirsNum = gridDirs.Num();

	if (gridDirsNum < 1) {
		gridDirs = { EGridDir::FRONT, EGridDir::RIGHT, EGridDir::UP, EGridDir::BACK, EGridDir::LEFT, EGridDir::DOWN };
		gridDirsNum = 6;
	}

	for (size_t i = 0; i < gridDirsNum; i++){
		nearbyChunks.Add(GetNearbyChunk(gridDirs[i]));
	}


	return nearbyChunks;
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


//ChunkTaks
ChunkTask::ChunkTask(AChunk * _chunk, bool _bSaveInsteadOfLoad){
	chunk = _chunk;
	bSaveInsteadOfLoad = _bSaveInsteadOfLoad;
}

ChunkTask::~ChunkTask(){
}

void ChunkTask::DoWork(){
	chunk->bIsTaskFinished = false;

	if (bSaveInsteadOfLoad) {
		chunk->SaveToFile();
	}
	else {
		if (!chunk->LoadFromFile()) {
			chunk->Create();
		}

		//chunk->GenerateTerrainMesh();
		//chunk->proceduralMesh->GenerateMesh(chunk);
		AChunk::OnEndTask(chunk);
	}
	


	chunk->bIsTaskFinished = true;
}
 