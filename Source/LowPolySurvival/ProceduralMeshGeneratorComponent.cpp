// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralMeshGeneratorComponent.h"
#include "DrawDebugHelpers.h"
#include "SimplexNoise.h"
#include "Chunk.h"
#include "KismetProceduralMeshLibrary.h"
#include "MyGameInstance.h"
#include "WorldGenerator.h"
#include "GridVoxelMeshGen.h"




UProceduralMeshGeneratorComponent::UProceduralMeshGeneratorComponent(const FObjectInitializer & ObjectInitializer) : UProceduralMeshComponent(ObjectInitializer){

	PrimaryComponentTick.bCanEverTick = true;
	
	
}

void UProceduralMeshGeneratorComponent::BeginPlay(){
	Super::BeginPlay();

	meshGen = NewObject<UGridMarchingCubesMeshGen>(this);
	//meshGen = NewObject<UGridVoxelMeshGen>(this);

}

void UProceduralMeshGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (meshGen->IsReady() && !bIsMeshGenerated) {
		CreateMesh(false); 
		
	}
}

void UProceduralMeshGeneratorComponent::BeginDestroy(){

	EnsureCompletion();
	
	Super::BeginDestroy();
}

void UProceduralMeshGeneratorComponent::EnsureCompletion(){
	if (meshGen) {
		meshGen->EnsureCompletion();
	}
}


void UProceduralMeshGeneratorComponent::GenerateMesh(const AChunk * chunk){

	if (!chunk) return;

	bIsMeshGenerated = false;
	 
	auto chunkGrid = *chunk->GetGridData();
	gridSize = FVector(chunkGrid.dims);

	FGridMeshParams params;
	params.chunk = chunk;
	meshGenData = meshGen->CreateMesh(chunkGrid, params);
}


void UProceduralMeshGeneratorComponent::UpdateMesh(const AChunk * chunk, const FIntVector &chunkBlockLoc){
	
	if (!meshGen->IsReady()) return;

	bIsMeshGenerated = false;

	FGridMeshParams params;
	params.chunk = chunk;
	meshGenData = meshGen->CreateMesh(*chunk->GetGridData(), params);
}


void UProceduralMeshGeneratorComponent::CreateMesh(bool bBorderNormalsOnly){
	//UE_LOG(LogTemp, Warning, TEXT("Create Mesh!"));

	if (bBorderNormalsOnly) { 
		FOccluderVertexArray meshNormals;
		TArray<FProcMeshTangent> meshTangents;

		for (size_t i = 0; i < meshGenData->vertexArray.Num(); i++) {

			if (!borderVertexIndecies.Contains(i)) {
				meshNormals.Add(meshGenData->normals[i]);
				meshTangents.Add(meshGenData->tangents[i]);


				//Draw Normals
				//DrawDebugLine(GetWorld(), GetComponentLocation() + vertexArray[i], GetComponentLocation() + normals[i] * 100 + vertexArray[i], FColor::Red, false, 60, 0, 1);
			}

		}

		ClearMeshSection(0);
		CreateMeshSection(0, meshVertexArray, meshTriangles, meshNormals, TArray<FVector2D>(), meshGenData->vertColors, meshTangents, true);
	}
	else {
		
		if (meshGenData) {
			ClearMeshSection(0);
			CreateMeshSection(0, meshGenData->vertexArray, meshGenData->triangles, meshGenData->normals, meshGenData->UVs, meshGenData->vertColors, meshGenData->tangents, true);
		}
		
	}

	bIsMeshGenerated = true;
	onGeneratedMesh.Broadcast();
}