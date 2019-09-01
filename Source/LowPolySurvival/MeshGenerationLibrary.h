// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProceduralMeshComponent.h"
#include "MeshGenerationLibrary.generated.h"



USTRUCT()
struct LOWPOLYSURVIVAL_API FMeshGenData {

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

	FString ToString() const {
		return "Verticies: " + FString::FromInt(vertexArray.Num()) + 
		", Triangles: " + FString::FromInt(triangles.Num()) + 
		", UVs: " + FString::FromInt(UVs.Num()) + 
		", Normals: " + FString::FromInt(normals.Num()) + 
		", Tangents: " + FString::FromInt(tangents.Num()) + 
		", Colors: " + FString::FromInt(vertColors.Num());
	}
};

/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UMeshGenerationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	


public:

	static void FindVertOverlaps(int32 testVertIndex, const TArray<FVector>& verts, TArray<int32>& OUT_vertOverlaps);

	static void CalculateNormalsAndTangents(FMeshGenData &meshGenData, float normalsMultiplier = 0.0f);
};
