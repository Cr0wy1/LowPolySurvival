// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary.h"
#include "Engine/Texture2D.h"
#include "Kismet/KismetRenderingLibrary.h" //DrawMaterialToCanvas
#include "Engine/Canvas.h"
#include "Engine.h"


UTexture2D* UFunctionLibrary::CreateTextureFromTextures(const TArray<UTexture2D*> textures) {

	if (textures.Num() < 1 || !textures[0]) return nullptr;


	int32 w = textures[0]->GetSizeX();
	int32 h = textures[0]->GetSizeY();
	int32 mDataSize = w * h * 4;
	int32 mDataSqrtSize = w * 4;

	uint8* mTextureColors = new uint8[mDataSize];
	uint8* mDynamicColors = new uint8[mDataSize];

	FUpdateTextureRegion2D * mUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, w, h);

	UTexture2D* dynamicTexture = UTexture2D::CreateTransient(w, h, EPixelFormat::PF_B8G8R8A8);
	dynamicTexture->AlphaCoverageThresholds = textures[0]->AlphaCoverageThresholds;
	dynamicTexture->MipGenSettings = textures[0]->MipGenSettings;
	dynamicTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	dynamicTexture->SRGB = true;
	dynamicTexture->AddToRoot();

	//First initial mDynamicColors
	FTexture2DMipMap& readMipFirst = textures[0]->PlatformData->Mips[0];

	void* Data = readMipFirst.BulkData.Lock(LOCK_READ_WRITE);
	//readMipFirst.BulkData.GetCopy((void**)&mTextureColors);
	FMemory::Memcpy(mDynamicColors, Data, mDataSize);
	readMipFirst.BulkData.Unlock();

	dynamicTexture->UpdateResource();
	

	int32 texturesNum = textures.Num();
	for (size_t t = 1; t < texturesNum; t++){

		FTexture2DMipMap& readMip = textures[t]->PlatformData->Mips[0];
		void* Data = readMip.BulkData.Lock(LOCK_READ_WRITE);
		//readMip.BulkData.GetCopy((void**)&mTextureColors);
		FMemory::Memcpy(mTextureColors, Data, mDataSize);
		readMip.BulkData.Unlock();

		dynamicTexture->UpdateResource();

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

	//dynamicTexture->UpdateResource();

	FMemory::Free(mTextureColors);
	
	dynamicTexture->UpdateTextureRegions(0, 1, mUpdateTextureRegion, mDataSqrtSize, (uint32)4, mDynamicColors, [](uint8* SrcData, const FUpdateTextureRegion2D* Regions) { delete Regions; FMemory::Free(SrcData); });

	return dynamicTexture;
}

UTexture2D * UFunctionLibrary::CreateColoredTexture(const UTexture2D * texture, const FLinearColor &color){

	if (!texture) return nullptr;


	int32 w = texture->GetSizeX();
	int32 h = texture->GetSizeY();
	int32 mDataSize = w * h * 4;
	int32 mDataSqrtSize = w * 4;

	//uint8* mTextureColors = new uint8[mDataSize];
	uint8* mDynamicColors = new uint8[mDataSize];

	FUpdateTextureRegion2D * mUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, w, h);

	UTexture2D* dynamicTexture = UTexture2D::CreateTransient(w, h, EPixelFormat::PF_B8G8R8A8);
	dynamicTexture->AlphaCoverageThresholds = texture->AlphaCoverageThresholds;
	dynamicTexture->MipGenSettings = texture->MipGenSettings;
	dynamicTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	dynamicTexture->SRGB = true;
	dynamicTexture->AddToRoot();
	

	//First initial mDynamicColors 
	FTexture2DMipMap& readMipFirst = texture->PlatformData->Mips[0];
	
	//readMipFirst.BulkData.GetCopy((void**)&mTextureColors);

	void* Data = readMipFirst.BulkData.Lock(LOCK_READ_WRITE);
	
	FMemory::Memcpy(mDynamicColors, Data, mDataSize);
	readMipFirst.BulkData.Unlock();

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

		mDynamicColors[blue] = (uint8)(b * color.B * 255.f);
		mDynamicColors[green] = (uint8)(g * color.G * 255.f); 
		mDynamicColors[red] = (uint8)(r * color.R * 255.f);
	
	} 
	
	
	//FMemory::Free(mTextureColors);
	
	dynamicTexture->UpdateTextureRegions(0, 1, mUpdateTextureRegion, mDataSqrtSize, (uint32)4, mDynamicColors, [](uint8* SrcData, const FUpdateTextureRegion2D* Regions) { delete Regions; delete[] SrcData; });
	
	return dynamicTexture;
}

UWorld * UFunctionLibrary::GetActiveWorld(UObject * worldContextObject){

	return GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull);
}

void UFunctionLibrary::CanvasTest() {
	UObject* WorldContextObject = nullptr;
	
	//FCanvas canvas()
	

	//UKismetRenderingLibrary::DrawMaterialToRenderTarget
}

