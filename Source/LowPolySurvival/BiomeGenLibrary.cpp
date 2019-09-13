// Fill out your copyright notice in the Description page of Project Settings.


#include "BiomeGenLibrary.h"

FBiomeDiagram FSBiomeDiagram::firstBiomeDiagram = FBiomeDiagram();
FBiomeDiagram FSBiomeDiagram::baseBiomeDiagram = FBiomeDiagram();


FBiomeData * FBiomeDiagram::GetBiome(float xPercent, float yPercent) const{
	xPercent = FMath::Clamp(xPercent, 0.0f, 0.9999f);
	yPercent = 0.9999f - FMath::Clamp(yPercent, 0.0f, 0.9999f);// minus 0.9999f because the diagram begins at bottem left

	int32 x = FMath::FloorToInt(xPercent * xNum);
	int32 y = FMath::FloorToInt(yPercent * yNum);

	if (!diagramData[y][x]) {
		UE_LOG(LogTemp, Warning, TEXT("Num: %i"), diagramData.Num());
	}
	

	//return FSBiome::DESERT;
	return diagramData[y][x];
}

FBiomeData * FBiomeDiagram::GetBiome(float xPercent, float yPercent, bool xInvert, bool yInvert) const{

	if (xInvert) {
		xPercent = 0.9999f - xPercent;
	}

	if (yInvert) {
		yPercent = 0.9999f - yPercent;
	}

	return GetBiome(xPercent, yPercent);
}

void FSBiomeDiagram::InitStaticMembers(){

	FSBiomeDiagram::firstBiomeDiagram = {
		{ FSBiome::RAINFOREST,		FSBiome::RAINFOREST,	FSBiome::SWAMP, FSBiome::SWAMP,		FSBiome::TAIGA,		FSBiome::TAIGA,		FSBiome::TUNDRA, FSBiome::TUNDRA },
		{ FSBiome::SEASONALFOREST,	FSBiome::SEASONALFOREST,FSBiome::FOREST,FSBiome::FOREST,	FSBiome::TAIGA,		FSBiome::TAIGA,		FSBiome::TUNDRA, FSBiome::TUNDRA },
		{ FSBiome::SAVANNA,			FSBiome::SAVANNA,		FSBiome::WOODS,	FSBiome::WOODS,		FSBiome::TAIGA,		FSBiome::TAIGA,		FSBiome::TUNDRA, FSBiome::TUNDRA },
		{ FSBiome::DESERT,			FSBiome::DESERT,		FSBiome::DESERT,FSBiome::GRASDESERT,FSBiome::GRASDESERT,FSBiome::GRASDESERT,FSBiome::TUNDRA, FSBiome::TUNDRA },
	};

	FSBiomeDiagram::baseBiomeDiagram = {
		{ FSBiome::STONE,			FSBiome::RAINFOREST,	FSBiome::GRASDESERT, FSBiome::GRASDESERT,FSBiome::TUNDRA,FSBiome::TUNDRA },
		{ FSBiome::SAVANNA,			FSBiome::SAVANNA,		FSBiome::FOREST,FSBiome::FOREST,FSBiome::TAIGA,	FSBiome::TAIGA},
		{ FSBiome::SAVANNA,			FSBiome::GRAS,			FSBiome::GRAS,	FSBiome::FOREST,FSBiome::FOREST,FSBiome::SWAMP },
		{ FSBiome::SAVANNA,			FSBiome::GRAS,			FSBiome::GRAS,	FSBiome::FOREST,FSBiome::FOREST,FSBiome::SWAMP },
		{ FSBiome::DESERT,			FSBiome::GRAS,			FSBiome::FOREST,FSBiome::FOREST,FSBiome::SWAMP,	FSBiome::SWAMP },
		{ FSBiome::OCEAN,			FSBiome::OCEAN,			FSBiome::OCEAN,	FSBiome::OCEAN,FSBiome::OCEAN,	FSBiome::OCEAN },
	};
}
