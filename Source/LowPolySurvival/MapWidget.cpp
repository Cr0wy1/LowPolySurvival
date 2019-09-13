// Fill out your copyright notice in the Description page of Project Settings.


#include "MapWidget.h"
#include "Image.h"
#include "Engine/Texture2D.h"
#include "SimplexNoise.h"



void UMapWidget::RenderMap() {
	
	int32 w = 100;
	int32 h = 100;
	int32 mDataSize = w * h * 4;
	int32 mDataSqrtSize = w * 4;

	//uint8* mTextureColors = new uint8[mDataSize];
	uint8* mDynamicColors = new uint8[mDataSize];

	FUpdateTextureRegion2D * mUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, w, h);

	UTexture2D* dynamicTexture = UTexture2D::CreateTransient(w, h, EPixelFormat::PF_B8G8R8A8);
	//dynamicTexture->AlphaCoverageThresholds = texture->AlphaCoverageThresholds;
	//dynamicTexture->MipGenSettings = texture->MipGenSettings;
	dynamicTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	dynamicTexture->SRGB = true;
	dynamicTexture->AddToRoot();

	dynamicTexture->UpdateResource();

	int32 pixelAmount = mDataSize / 4;
	for (int32 i = 0; i < pixelAmount; ++i) {
		int32 blue = i * 4 + 0;
		int32 green = i * 4 + 1;
		int32 red = i * 4 + 2;
		int32 alpha = i * 4 + 3;

		float b = (float)mDynamicColors[blue] / 255.f;
		float g = (float)mDynamicColors[green] / 255.f;
		float r = (float)mDynamicColors[red] / 255.f;
		float a = (float)mDynamicColors[alpha] / 255.f;

		int32 x = i % 100;
		int32 y = (i - x) / 100;
		uint8 noiseColor = 255 * Noise(x, y, 1, 1);

		mDynamicColors[blue] = noiseColor;
		mDynamicColors[green] = noiseColor; 
		mDynamicColors[red] = noiseColor;
		mDynamicColors[alpha] = 255;

	}


	//FMemory::Free(mTextureColors);

	dynamicTexture->UpdateTextureRegions(0, 1, mUpdateTextureRegion, mDataSqrtSize, (uint32)4, mDynamicColors, [](uint8* SrcData, const FUpdateTextureRegion2D* Regions) { delete Regions; delete[] SrcData; });

	mapImage->SetBrushFromTexture(dynamicTexture);

}