// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridMeshGen.h"
#include "GridMarchingCubesMeshGen.generated.h"




class AChunk;


USTRUCT()
struct LOWPOLYSURVIVAL_API FMarchCube {

	GENERATED_BODY()

	static const TArray<TArray<uint8>> edgeCorners;
	static const TArray<FVector> relativEdgeCenters;
	static const TArray<FVector> relativCorners;
	static const TArray<TArray<uint8>> triTable;

	TArray<FBlockData, TInlineAllocator<8>> corners;
	TArray<FColor, TInlineAllocator<8>> cornerColors;

	uint8 GetCubeIndex(float isolevel) const;
	FLinearColor GetEdgeColor(uint8 edgeIndex) const;
	FVector GetLerpedVertexpPos(uint8 edgeIndex) const;
}; 
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UGridMarchingCubesMeshGen : public UGridMeshGen
{
	GENERATED_BODY()


public:

	virtual FMeshGenData* CreateMesh(const FBlockGrid &blockGrid, const FGridMeshParams &param = FGridMeshParams()) override;
	virtual FMeshGenData* UpdateMesh(const FBlockGrid &blockGrid, const FIntVector &blockLoc, const FGridMeshParams &param = FGridMeshParams()) override;

protected:

	float isolevel = 0.5f;

	TArray<int32> borderVertexIndecies;
	//For calculate normals and tangents
	FOccluderVertexArray meshVertexArray;
	TArray<int32> meshTriangles;
	TArray<FVector2D> meshUV;

	TArray<TArray<TArray<FMarchCube>>> marchCubes;

	void MarchingCubes(bool bBorderNormalsOnly = true);
	void CreateMarchCubes(const AChunk* chunk);

	virtual void DoTaskWork(bool bUpdateOnly, const FBlockGrid &blockGrid) override;

};
