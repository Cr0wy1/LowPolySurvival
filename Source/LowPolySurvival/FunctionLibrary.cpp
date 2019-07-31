// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary.h"
#include "Engine/Texture2D.h"

UTexture2D* UFunctionLibrary::CreateTextureFromTextures(const TArray<UTexture2D*> textures) {

	if (textures.Num() < 1 || !textures[0]) return nullptr;


	int32 w = textures[0]->GetSizeX();
	int32 h = textures[0]->GetSizeY();
	int32 mDataSize = w * h * 4;
	int32 mDataSqrtSize = w * 4;

	uint8* mTextureColors = new uint8[mDataSize];
	uint8* mDynamicColors = new uint8[mDataSize];

	FUpdateTextureRegion2D * mUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, w, h);

	UTexture2D* dynamicTexture = UTexture2D::CreateTransient(w, h);
	dynamicTexture->MipGenSettings = textures[0]->MipGenSettings;
	dynamicTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	dynamicTexture->SRGB = 1;
	dynamicTexture->AddToRoot();
	dynamicTexture->UpdateResource();

	//First initial mDynamicColors
	FTexture2DMipMap& readMipFirst = textures[0]->PlatformData->Mips[0];
	readMipFirst.BulkData.GetCopy((void**)&mTextureColors);
	FMemory::Memcpy(mDynamicColors, mTextureColors, mDataSize);

	int32 texturesNum = textures.Num();
	for (size_t i = 1; i < texturesNum; i++){

		FTexture2DMipMap& readMip = textures[i]->PlatformData->Mips[0];
		readMip.BulkData.GetCopy((void**)&mTextureColors);

		//UE_LOG(LogTemp, Warning, TEXT("colors: %s"), *FColor(mDynamicColors[0], mDynamicColors[1], mDynamicColors[2], mDynamicColors[3]).ToString());

		int32 pixelAmount = mDataSize / 4;
		for (int32 i = 0; i < pixelAmount; ++i)
		{
			int32 blue = i * 4 + 0;
			int32 green = i * 4 + 1;
			int32 red = i * 4 + 2;
			int32 alpha = i * 4 + 3;

			if (mTextureColors[alpha] > uint8(0)) { 
				//float b = (float)mDynamicColors[blue] / 255;
				//float g = (float)mDynamicColors[green] / 255;
				//float r = (float)mDynamicColors[red] / 255;
				//float a = (float)mDynamicColors[alpha] / 255;

				//float b2 = (float)mTextureColors[blue] / 255;
				//float g2 = (float)mTextureColors[green] / 255;
				//float r2 = (float)mTextureColors[red] / 255;
				//float a2 = (float)mTextureColors[alpha] / 255;
				 
				mDynamicColors[blue] = mTextureColors[blue]; 
				mDynamicColors[green] = mTextureColors[green];   
				mDynamicColors[red] = mTextureColors[red];
				 
				
				mDynamicColors[alpha] = mTextureColors[alpha] > mDynamicColors[alpha] ? mTextureColors[alpha] : mDynamicColors[alpha];
				
				
			}

		}

	}

	FMemory::Free(mTextureColors);
	
	dynamicTexture->UpdateTextureRegions(0, 1, mUpdateTextureRegion, mDataSqrtSize, (uint32)4, mDynamicColors, [](uint8* SrcData, const FUpdateTextureRegion2D* Regions) { delete Regions; FMemory::Free(SrcData); });
	//UpdateTextureRegions(dynamicTexture, 0, 1, mUpdateTextureRegion, mDataSqrtSize, (uint32)4, mDynamicColors, true);

	return dynamicTexture;
}

UTexture2D * UFunctionLibrary::CreateColoredTexture(UTexture2D * texture, const FLinearColor &color){

	if (!texture) return nullptr;


	int32 w = texture->GetSizeX();
	int32 h = texture->GetSizeY();
	int32 mDataSize = w * h * 4;
	int32 mDataSqrtSize = w * 4;

	uint8* mTextureColors = new uint8[mDataSize];
	uint8* mDynamicColors = new uint8[mDataSize];

	FUpdateTextureRegion2D * mUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, w, h);

	UTexture2D* dynamicTexture = UTexture2D::CreateTransient(w, h);
	//dynamicTexture->MipGenSettings = texture->MipGenSettings;
	dynamicTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	dynamicTexture->SRGB = 1;
	dynamicTexture->AddToRoot();
	dynamicTexture->UpdateResource();

	//First initial mDynamicColors
	FTexture2DMipMap& readMipFirst = texture->PlatformData->Mips[0];
	readMipFirst.BulkData.GetCopy((void**)&mTextureColors);
	FMemory::Memcpy(mDynamicColors, mTextureColors, mDataSize);

	int32 pixelAmount = mDataSize / 4;
	for (int32 i = 0; i < pixelAmount; ++i) {
		int32 blue = i * 4 + 0;
		int32 green = i * 4 + 1;
		int32 red = i * 4 + 2;
		int32 alpha = i * 4 + 3;

		float b = (float)mTextureColors[blue] / 255.f;
		float g = (float)mTextureColors[green] / 255.f;
		float r = (float)mTextureColors[red] / 255.f;
		float a = (float)mTextureColors[alpha] / 255.f;

		//mDynamicColors[blue] = (uint8)(b * color.B * 255.f);
		//mDynamicColors[green] = (uint8)(g * color.G * 255.f);
		//mDynamicColors[red] = (uint8)(r * color.R * 255.f);

		//mDynamicColors[alpha] = mTextureColors[alpha];

		//mDynamicColors[blue] = 255;
		//mDynamicColors[green] = 255;
		//mDynamicColors[red] = rand() % 255;

		//mDynamicColors[alpha] = 255;
	
	}

	FMemory::Free(mTextureColors);

	dynamicTexture->UpdateTextureRegions(0, 1, mUpdateTextureRegion, mDataSqrtSize, (uint32)4, mDynamicColors, [](uint8* SrcData, const FUpdateTextureRegion2D* Regions) { delete Regions; FMemory::Free(SrcData); });
	
	return dynamicTexture;
}
