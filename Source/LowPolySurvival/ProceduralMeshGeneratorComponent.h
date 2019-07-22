// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameStructs.h"
#include "HAL/Runnable.h"
#include "ProceduralMeshGeneratorComponent.generated.h"


class UGridComponent;
class AChunk;


USTRUCT()
struct LOWPOLYSURVIVAL_API FProcMeshData {

	GENERATED_BODY()

	FOccluderVertexArray vertexArray;
	TArray<int32> triangles;
	TArray<FVector2D> UVs;
	FOccluderVertexArray normals;
	TArray<FProcMeshTangent> tangents;
	TArray<FColor> vertColors;

	bool bIsReady = false;

	void Reset() {
		vertexArray.Reset();
		triangles.Reset();
		UVs.Reset();
		normals.Reset();
		tangents.Reset();
		vertColors.Reset();
	}
};


USTRUCT()
struct LOWPOLYSURVIVAL_API FMarchCube {

	GENERATED_BODY()

	TArray<FBlockData, TInlineAllocator<8>> corners;
	TArray<FColor, TInlineAllocator<8>> cornerColors;

	TArray<TArray<uint8>> edgeCorners = {
		{ 0,1 },
		{ 2,1 },
		{ 3,2 },
		{ 3,0 },
		{ 4,5 },
		{ 6,5 },
		{ 7,6 },
		{ 7,4 },
		{ 0,4 },
		{ 1,5 },
		{ 2,6 },
		{ 3,7 },
	};

	uint8 GetCubeIndex() const{
		uint8 cubeIndex = 0;
		for (size_t i = 0; i < 8; i++){
			if (corners[i].blockId < 1) {
				cubeIndex |= 1 << i;
			}
		}
		return cubeIndex;
	}

	FLinearColor GetColorFromBlockId(int32 blockId) const {
		
		if (blockId == 1) {
			return FColor::Black;
		}
		else if (blockId == 2) {
			return FColor(139, 69, 19);
		}
		else if (blockId == 3) {
			return FColor::Green;
		}

		return FColor::Purple;
	}

	FLinearColor GetEdgeColor(uint8 edgeIndex) const{

		uint8 cornerIndex1 = edgeCorners[edgeIndex][0];
		uint8 cornerIndex2 = edgeCorners[edgeIndex][1];

		const FBlockData* corner1 = &corners[cornerIndex1];
		const FBlockData* corner2 = &corners[cornerIndex2];

		FLinearColor finalColor;

		if (corner1->blockId != 0) {
			finalColor = GetColorFromBlockId(corner1->blockId);

			if (corner2->blockId != 0) {
				FLinearColor color2 = GetColorFromBlockId(corner2->blockId);
				finalColor = FLinearColor::LerpUsingHSV(finalColor, color2, 0.5f);
			}
		}
		else {
			finalColor = GetColorFromBlockId(corner2->blockId);
		}
		
		return finalColor;
	}


};
/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LOWPOLYSURVIVAL_API UProceduralMeshGeneratorComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()


protected:

	TArray<int32> borderVertexIndecies;

	//For calculate normals and tangents
	FOccluderVertexArray meshVertexArray;
	TArray<int32> meshTriangles;
	TArray<FVector2D> meshUV;

	bool bIsMeshGenerated = false;

	float blockSize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marching Cube")
	FVector gridSize = { 7,7,7 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marching Cube")
		float offset = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Marching Cube")
		bool bDrawDebug = false;


	TArray<TArray<TArray<FMarchCube>>> marchCubes;

	void BeginPlay();

	
	void CreateMarchCubes(const TArray<TArray<TArray<FBlockData>>>& blockGrid);
	

	void CreateMesh(bool bBorderNormalsOnly);

	

	

public:

	FProcMeshData procMeshData;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TQueue<FProcMeshData> procQueue;

	UProceduralMeshGeneratorComponent(const FObjectInitializer & ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, Category = "MeshGeneration")
	void GenerateMesh(const UGridComponent* gridComp);

	void GenerateMesh(const AChunk* chunk);
	void GenerateMesh(const TArray<TArray<TArray<FBlockData>>>& blockGrid);

	void UpdateMesh(const AChunk* chunk, const FIntVector &chunkBlockLoc);
	void UpdateMesh(const TArray<TArray<TArray<FBlockData>>>& blockGrid, FIntVector blockLocation);

	void MarchingCubes(bool bBorderNormalsOnly = true);
	void CreateMarchCubes(const AChunk* chunk);
	

	static void CalculateNormalsAndTangents(FProcMeshData &procMeshData);

protected:

	TArray<TArray<uint8>> triTable =
	{	
		{  },
		{ 0, 8, 3 },
		{ 0, 1, 9 },
		{ 1, 8, 3, 9, 8, 1 },
		{ 1, 2, 10 },
		{ 0, 8, 3, 1, 2, 10 },
		{ 9, 2, 10, 0, 2, 9 },
		{ 2, 8, 3, 2, 10, 8, 10, 9, 8 },
		{ 3, 11, 2 },
		{ 0, 11, 2, 8, 11, 0 },
		{ 1, 9, 0, 2, 3, 11 },
		{ 1, 11, 2, 1, 9, 11, 9, 8, 11 },
		{ 3, 10, 1, 11, 10, 3 },
		{ 0, 10, 1, 0, 8, 10, 8, 11, 10 },
		{ 3, 9, 0, 3, 11, 9, 11, 10, 9 },
		{ 9, 8, 10, 10, 8, 11 },
		{ 4, 7, 8 },
		{ 4, 3, 0, 7, 3, 4 },
		{ 0, 1, 9, 8, 4, 7 },
		{ 4, 1, 9, 4, 7, 1, 7, 3, 1 },
		{ 1, 2, 10, 8, 4, 7 },
		{ 3, 4, 7, 3, 0, 4, 1, 2, 10 },
		{ 9, 2, 10, 9, 0, 2, 8, 4, 7 },
		{ 2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4 },
		{ 8, 4, 7, 3, 11, 2 },
		{ 11, 4, 7, 11, 2, 4, 2, 0, 4 },
		{ 9, 0, 1, 8, 4, 7, 2, 3, 11 },
		{ 4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1 },
		{ 3, 10, 1, 3, 11, 10, 7, 8, 4 },
		{ 1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4 },
		{ 4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3 },
		{ 4, 7, 11, 4, 11, 9, 9, 11, 10 },
		{ 9, 5, 4 },
		{ 9, 5, 4, 0, 8, 3 },
		{ 0, 5, 4, 1, 5, 0 },
		{ 8, 5, 4, 8, 3, 5, 3, 1, 5 },
		{ 1, 2, 10, 9, 5, 4 },
		{ 3, 0, 8, 1, 2, 10, 4, 9, 5 },
		{ 5, 2, 10, 5, 4, 2, 4, 0, 2 },
		{ 2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8 },
		{ 9, 5, 4, 2, 3, 11 },
		{ 0, 11, 2, 0, 8, 11, 4, 9, 5 },
		{ 0, 5, 4, 0, 1, 5, 2, 3, 11 },
		{ 2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5 },
		{ 10, 3, 11, 10, 1, 3, 9, 5, 4 },
		{ 4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10 },
		{ 5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3 },
		{ 5, 4, 8, 5, 8, 10, 10, 8, 11 },
		{ 9, 7, 8, 5, 7, 9 },
		{ 9, 3, 0, 9, 5, 3, 5, 7, 3 },
		{ 0, 7, 8, 0, 1, 7, 1, 5, 7 },
		{ 1, 5, 3, 3, 5, 7 },
		{ 9, 7, 8, 9, 5, 7, 10, 1, 2 },
		{ 10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3 },
		{ 8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2 },
		{ 2, 10, 5, 2, 5, 3, 3, 5, 7 },
		{ 7, 9, 5, 7, 8, 9, 3, 11, 2 },
		{ 9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11 },
		{ 2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7 },
		{ 11, 2, 1, 11, 1, 7, 7, 1, 5 },
		{ 9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11 },
		{ 5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0 },
		{ 11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0 },
		{ 11, 10, 5, 7, 11, 5 },
		{ 10, 6, 5 },
		{ 0, 8, 3, 5, 10, 6 },
		{ 9, 0, 1, 5, 10, 6 },
		{ 1, 8, 3, 1, 9, 8, 5, 10, 6 },
		{ 1, 6, 5, 2, 6, 1 },
		{ 1, 6, 5, 1, 2, 6, 3, 0, 8 },
		{ 9, 6, 5, 9, 0, 6, 0, 2, 6 },
		{ 5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8 },
		{ 2, 3, 11, 10, 6, 5 },
		{ 11, 0, 8, 11, 2, 0, 10, 6, 5 },
		{ 0, 1, 9, 2, 3, 11, 5, 10, 6 },
		{ 5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11 },
		{ 6, 3, 11, 6, 5, 3, 5, 1, 3 },
		{ 0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6 },
		{ 3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9 },
		{ 6, 5, 9, 6, 9, 11, 11, 9, 8 },
		{ 5, 10, 6, 4, 7, 8 },
		{ 4, 3, 0, 4, 7, 3, 6, 5, 10 },
		{ 1, 9, 0, 5, 10, 6, 8, 4, 7 },
		{ 10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4 },
		{ 6, 1, 2, 6, 5, 1, 4, 7, 8 },
		{ 1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7 },
		{ 8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6 },
		{ 7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9 },
		{ 3, 11, 2, 7, 8, 4, 10, 6, 5 },
		{ 5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11 },
		{ 0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6 },
		{ 9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6 },
		{ 8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6 },
		{ 5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11 },
		{ 0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7 },
		{ 6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9 },
		{ 10, 4, 9, 6, 4, 10 },
		{ 4, 10, 6, 4, 9, 10, 0, 8, 3 },
		{ 10, 0, 1, 10, 6, 0, 6, 4, 0 },
		{ 8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10 },
		{ 1, 4, 9, 1, 2, 4, 2, 6, 4 },
		{ 3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4 },
		{ 0, 2, 4, 4, 2, 6 },
		{ 8, 3, 2, 8, 2, 4, 4, 2, 6 },
		{ 10, 4, 9, 10, 6, 4, 11, 2, 3 },
		{ 0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6 },
		{ 3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10 },
		{ 6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1 },
		{ 9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3 },
		{ 8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1 },
		{ 3, 11, 6, 3, 6, 0, 0, 6, 4 },
		{ 6, 4, 8, 11, 6, 8 },
		{ 7, 10, 6, 7, 8, 10, 8, 9, 10 },
		{ 0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10 },
		{ 10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0 },
		{ 10, 6, 7, 10, 7, 1, 1, 7, 3 },
		{ 1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7 },
		{ 2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9 },
		{ 7, 8, 0, 7, 0, 6, 6, 0, 2 },
		{ 7, 3, 2, 6, 7, 2 },
		{ 2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7 },
		{ 2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7 },
		{ 1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11 },
		{ 11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1 },
		{ 8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6 },
		{ 0, 9, 1, 11, 6, 7 },
		{ 7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0 },
		{ 7, 11, 6 },
		{ 7, 6, 11 },
		{ 3, 0, 8, 11, 7, 6 },
		{ 0, 1, 9, 11, 7, 6 },
		{ 8, 1, 9, 8, 3, 1, 11, 7, 6 },
		{ 10, 1, 2, 6, 11, 7 },
		{ 1, 2, 10, 3, 0, 8, 6, 11, 7 },
		{ 2, 9, 0, 2, 10, 9, 6, 11, 7 },
		{ 6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8 },
		{ 7, 2, 3, 6, 2, 7 },
		{ 7, 0, 8, 7, 6, 0, 6, 2, 0 },
		{ 2, 7, 6, 2, 3, 7, 0, 1, 9 },
		{ 1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6 },
		{ 10, 7, 6, 10, 1, 7, 1, 3, 7 },
		{ 10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8 },
		{ 0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7 },
		{ 7, 6, 10, 7, 10, 8, 8, 10, 9 },
		{ 6, 8, 4, 11, 8, 6 },
		{ 3, 6, 11, 3, 0, 6, 0, 4, 6 },
		{ 8, 6, 11, 8, 4, 6, 9, 0, 1 },
		{ 9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6 },
		{ 6, 8, 4, 6, 11, 8, 2, 10, 1 },
		{ 1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6 },
		{ 4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9 },
		{ 10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3 },
		{ 8, 2, 3, 8, 4, 2, 4, 6, 2 },
		{ 0, 4, 2, 4, 6, 2 },
		{ 1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8 },
		{ 1, 9, 4, 1, 4, 2, 2, 4, 6 },
		{ 8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1 },
		{ 10, 1, 0, 10, 0, 6, 6, 0, 4 },
		{ 4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3 },
		{ 10, 9, 4, 6, 10, 4 },
		{ 4, 9, 5, 7, 6, 11 },
		{ 0, 8, 3, 4, 9, 5, 11, 7, 6 },
		{ 5, 0, 1, 5, 4, 0, 7, 6, 11 },
		{ 11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5 },
		{ 9, 5, 4, 10, 1, 2, 7, 6, 11 },
		{ 6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5 },
		{ 7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2 },
		{ 3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6 },
		{ 7, 2, 3, 7, 6, 2, 5, 4, 9 },
		{ 9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7 },
		{ 3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0 },
		{ 6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8 },
		{ 9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7 },
		{ 1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4 },
		{ 4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10 },
		{ 7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10 },
		{ 6, 9, 5, 6, 11, 9, 11, 8, 9 },
		{ 3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5 },
		{ 0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11 },
		{ 6, 11, 3, 6, 3, 5, 5, 3, 1 },
		{ 1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6 },
		{ 0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10 },
		{ 11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5 },
		{ 6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3 },
		{ 5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2 },
		{ 9, 5, 6, 9, 6, 0, 0, 6, 2 },
		{ 1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8 },
		{ 1, 5, 6, 2, 1, 6 },
		{ 1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6 },
		{ 10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0 },
		{ 0, 3, 8, 5, 6, 10 },
		{ 10, 5, 6 },
		{ 11, 5, 10, 7, 5, 11 },
		{ 11, 5, 10, 11, 7, 5, 8, 3, 0 },
		{ 5, 11, 7, 5, 10, 11, 1, 9, 0 },
		{ 10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1 },
		{ 11, 1, 2, 11, 7, 1, 7, 5, 1 },
		{ 0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11 },
		{ 9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7 },
		{ 7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2 },
		{ 2, 5, 10, 2, 3, 5, 3, 7, 5 },
		{ 8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5 },
		{ 9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2 },
		{ 9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2 },
		{ 1, 3, 5, 3, 7, 5 },
		{ 0, 8, 7, 0, 7, 1, 1, 7, 5 },
		{ 9, 0, 3, 9, 3, 5, 5, 3, 7 },
		{ 9, 8, 7, 5, 9, 7 },
		{ 5, 8, 4, 5, 10, 8, 10, 11, 8 },
		{ 5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0 },
		{ 0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5 },
		{ 10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4 },
		{ 2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8 },
		{ 0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11 },
		{ 0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5 },
		{ 9, 4, 5, 2, 11, 3 },
		{ 2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4 },
		{ 5, 10, 2, 5, 2, 4, 4, 2, 0 },
		{ 3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9 },
		{ 5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2 },
		{ 8, 4, 5, 8, 5, 3, 3, 5, 1 },
		{ 0, 4, 5, 1, 0, 5 },
		{ 8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5 },
		{ 9, 4, 5 },
		{ 4, 11, 7, 4, 9, 11, 9, 10, 11 },
		{ 0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11 },
		{ 1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11 },
		{ 3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4 },
		{ 4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2 },
		{ 9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3 },
		{ 11, 7, 4, 11, 4, 2, 2, 4, 0 },
		{ 11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4 },
		{ 2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9 },
		{ 9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7 },
		{ 3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10 },
		{ 1, 10, 2, 8, 7, 4 },
		{ 4, 9, 1, 4, 1, 7, 7, 1, 3 },
		{ 4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1 },
		{ 4, 0, 3, 7, 4, 3 },
		{ 4, 8, 7 },
		{ 9, 10, 8, 10, 11, 8 },
		{ 3, 0, 9, 3, 9, 11, 11, 9, 10 },
		{ 0, 1, 10, 0, 10, 8, 8, 10, 11 },
		{ 3, 1, 10, 11, 3, 10 },
		{ 1, 2, 11, 1, 11, 9, 9, 11, 8 },
		{ 3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9 },
		{ 0, 2, 11, 8, 0, 11 },
		{ 3, 2, 11 },
		{ 2, 3, 8, 2, 8, 10, 10, 8, 9 },
		{ 9, 10, 2, 0, 9, 2 },
		{ 2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8 },
		{ 1, 10, 2 },
		{ 1, 3, 8, 9, 1, 8 },
		{ 0, 9, 1 },
		{ 0, 3, 8 },
		{  } 
	};

	TArray<FVector> relativEdgeCenters = {
		FVector( 0.5, 0.0,-0.5),
		FVector( 0.0, 0.5,-0.5),
		FVector(-0.5, 0.0,-0.5),
		FVector( 0.0,-0.5,-0.5),
		FVector( 0.5, 0.0, 0.5),
		FVector( 0.0, 0.5, 0.5),
		FVector(-0.5, 0.0, 0.5),
		FVector( 0.0,-0.5, 0.5),
		FVector( 0.5,-0.5, 0.0),
		FVector( 0.5, 0.5, 0.0),
		FVector(-0.5, 0.5, 0.0),
		FVector(-0.5,-0.5, 0.0),
	};


};

class ProcMeshTask : public FNonAbandonableTask {

	
	UProceduralMeshGeneratorComponent* meshGenerator;
	const AChunk * chunk;
	bool bUpdateOnly = false;

public:
	ProcMeshTask(UProceduralMeshGeneratorComponent* _meshGenerator, const AChunk * _chunk, bool _bUpdateOnly);
	~ProcMeshTask();

	void DoWork();


	FORCEINLINE TStatId GetStatId() const { 
		RETURN_QUICK_DECLARE_CYCLE_STAT(ExampleAutoDeleteAsyncTask, STATGROUP_ThreadPoolAsyncTasks); 
	}

};
