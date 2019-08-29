// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshGenerationLibrary.h"


void UMeshGenerationLibrary::FindVertOverlaps(int32 testVertIndex, const TArray<FVector>& verts, TArray<int32>& OUT_vertOverlaps){
	// Check if Verts is empty or test is outside range
	if (testVertIndex < verts.Num()){
		const FVector TestVert = verts[testVertIndex];

		for (int32 VertIdx = 0; VertIdx < verts.Num(); VertIdx++){
			// First see if we overlap, and smoothing groups are the same
			if (TestVert.Equals(verts[VertIdx])){
				// If it, so we are at least considered an 'overlap' for normal gen
				OUT_vertOverlaps.Add(VertIdx);
			}
		}
	}
}


void UMeshGenerationLibrary::CalculateNormalsAndTangents(FMeshGenData &meshGenData, float normalsMultiplier) {

	//TODO Makse it optional
	//return;

	
	if (meshGenData.vertexArray.Num() == 0)
	{
		return;
	}

	// Number of triangles 
	const int32 NumTris = meshGenData.triangles.Num() / 3;
	// Number of verts
	const int32 NumVerts = meshGenData.vertexArray.Num();

	// Map of vertex to triangles in Triangles array
	TMultiMap<int32, int32> VertToTriMap;
	// Map of vertex to triangles to consider for normal calculation
	TMultiMap<int32, int32> VertToTriSmoothMap;

	// Normal/tangents for each face
	TArray<FVector> FaceTangentX, FaceTangentY, FaceTangentZ;
	FaceTangentX.AddUninitialized(NumTris);
	FaceTangentY.AddUninitialized(NumTris);
	FaceTangentZ.AddUninitialized(NumTris);

	// Iterate over triangles
	for (int TriIdx = 0; TriIdx < NumTris; TriIdx++)
	{
		int32 CornerIndex[3];
		FVector P[3];

		for (int32 CornerIdx = 0; CornerIdx < 3; CornerIdx++)
		{
			// Find vert index (clamped within range)
			int32 VertIndex = FMath::Min(meshGenData.triangles[(TriIdx * 3) + CornerIdx], NumVerts - 1);

			CornerIndex[CornerIdx] = VertIndex;
			P[CornerIdx] = meshGenData.vertexArray[VertIndex];

			// Find/add this vert to index buffer
			TArray<int32> VertOverlaps;
			FindVertOverlaps(VertIndex, meshGenData.vertexArray, VertOverlaps);

			// Remember which triangles map to this vert
			VertToTriMap.AddUnique(VertIndex, TriIdx);
			VertToTriSmoothMap.AddUnique(VertIndex, TriIdx);

			// Also update map of triangles that 'overlap' this vert (ie don't match UV, but do match smoothing) and should be considered when calculating normal
			for (int32 OverlapIdx = 0; OverlapIdx < VertOverlaps.Num(); OverlapIdx++)
			{
				// For each vert we overlap..
				int32 OverlapVertIdx = VertOverlaps[OverlapIdx];

				// Add this triangle to that vert
				VertToTriSmoothMap.AddUnique(OverlapVertIdx, TriIdx);

				// And add all of its triangles to us
				TArray<int32> OverlapTris;
				VertToTriMap.MultiFind(OverlapVertIdx, OverlapTris);
				for (int32 OverlapTriIdx = 0; OverlapTriIdx < OverlapTris.Num(); OverlapTriIdx++)
				{
					VertToTriSmoothMap.AddUnique(VertIndex, OverlapTris[OverlapTriIdx]);
				}
			}
		}

		// Calculate triangle edge vectors and normal
		const FVector Edge21 = P[1] - P[2];
		const FVector Edge20 = P[0] - P[2];
		const FVector TriNormal = (Edge21 ^ Edge20).GetSafeNormal();


		FaceTangentX[TriIdx] = Edge20.GetSafeNormal();
		FaceTangentY[TriIdx] = (FaceTangentX[TriIdx] ^ TriNormal).GetSafeNormal();


		FaceTangentZ[TriIdx] = TriNormal;
	}


	// Arrays to accumulate tangents into
	TArray<FVector> VertexTangentXSum, VertexTangentYSum, VertexTangentZSum;
	VertexTangentXSum.AddZeroed(NumVerts);
	VertexTangentYSum.AddZeroed(NumVerts);
	VertexTangentZSum.AddZeroed(NumVerts);

	// For each vertex..
	for (int VertxIdx = 0; VertxIdx < meshGenData.vertexArray.Num(); VertxIdx++)
	{
		// Find relevant triangles for normal
		TArray<int32> SmoothTris;
		VertToTriSmoothMap.MultiFind(VertxIdx, SmoothTris);

		int32 mainTriIdx = *VertToTriMap.Find(VertxIdx);
		VertexTangentZSum[VertxIdx] = FaceTangentZ[mainTriIdx];

		for (int i = 0; i < SmoothTris.Num(); i++)
		{
			int32 TriIdx = SmoothTris[i];

			if (TriIdx != mainTriIdx) {
				VertexTangentZSum[VertxIdx] += FaceTangentZ[TriIdx] * normalsMultiplier;
			}

		}


		// Find relevant triangles for tangents  
		TArray<int32> TangentTris;
		VertToTriMap.MultiFind(VertxIdx, TangentTris);

		for (int i = 0; i < TangentTris.Num(); i++)
		{
			int32 TriIdx = TangentTris[i];
			VertexTangentXSum[VertxIdx] += FaceTangentX[TriIdx];
			VertexTangentYSum[VertxIdx] += FaceTangentY[TriIdx];
		}
	}

	// Finally, normalize tangents and build output arrays

	meshGenData.normals.Reset();
	meshGenData.normals.AddUninitialized(NumVerts);

	meshGenData.tangents.Reset();
	meshGenData.tangents.AddUninitialized(NumVerts);

	int32 innerIndex = 0;
	for (int VertxIdx = 0; VertxIdx < NumVerts; VertxIdx++)
	{

		//if (!borderVertexIndecies.Contains(VertxIdx)) {


		FVector& TangentX = VertexTangentXSum[VertxIdx];
		FVector& TangentY = VertexTangentYSum[VertxIdx];
		FVector& TangentZ = VertexTangentZSum[VertxIdx];

		TangentX.Normalize();
		TangentZ.Normalize();

		meshGenData.normals[VertxIdx] = TangentZ;

		// Use Gram-Schmidt orthogonalization to make sure X is orth with Z
		TangentX -= TangentZ * (TangentZ | TangentX);
		TangentX.Normalize();

		// See if we need to flip TangentY when generating from cross product
		const bool bFlipBitangent = ((TangentZ ^ TangentX) | TangentY) < 0.f;

		meshGenData.tangents[VertxIdx] = FProcMeshTangent(TangentX, bFlipBitangent);

		innerIndex++;
		//}
	}
}
