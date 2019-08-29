// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameStructs.h"
#include "HAL/Runnable.h"
#include "GridMarchingCubesMeshGen.h"
#include "ProceduralMeshGeneratorComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGeneratedMesh);


class UGridComponent;
class AChunk;

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LOWPOLYSURVIVAL_API UProceduralMeshGeneratorComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

protected:

	UGridMeshGen * meshGen;

	TArray<int32> borderVertexIndecies;

	//For calculate normals and tangents
	FOccluderVertexArray meshVertexArray;
	TArray<int32> meshTriangles;
	TArray<FVector2D> meshUV;

	bool bIsMeshGenerated = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marching Cube")
	FVector gridSize = { 7,7,7 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marching Cube")
		bool bDrawDebug = false;


	TArray<TArray<TArray<FMarchCube>>> marchCubes;

	void BeginPlay();
	
	void CreateMesh(bool bBorderNormalsOnly);

public:

	FOnGeneratedMesh onGeneratedMesh;

	FMeshGenData* meshGenData;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UProceduralMeshGeneratorComponent(const FObjectInitializer & ObjectInitializer);

	void GenerateMesh(const AChunk* chunk);
	void UpdateMesh(const AChunk* chunk, const FIntVector &chunkBlockLoc);

};
