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



// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComponent;
}

// Called when the game starts or when spawned
void AChunk::BeginPlay(){
	Super::BeginPlay();
	
	gameInstance = GetGameInstance<UMyGameInstance>();
}

void AChunk::TopDownTrace(){

	FWorldInfo const* worldInfo = gameInstance->GetWorldInfo();
	
	FCollisionQueryParams params;
	params.bReturnPhysicalMaterial = true;

	TArray<FHitResult> hitResults;
	FVector start = GetActorLocation() + FVector(0, 0, worldInfo->buildHeight);
	FVector end = GetActorLocation() + FVector(0, 0, worldInfo->deathZone);
	GetWorld()->LineTraceMultiByChannel(hitResults, start, end, ECollisionChannel::ECC_WorldStatic, params);

	DrawDebugLine(GetWorld(), start, end, FColor::Green, true, 100, 0, 100);

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

// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChunk::Create(FVector2D _chunkLoc){
	chunkLoc = _chunkLoc;
	float chunkSize = gameInstance->GetWorldInfo()->chunkSize;

	float baseSpawnChance = 0.2f;

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

