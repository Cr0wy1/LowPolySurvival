// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemStackWidget.h"
#include "Image.h"
#include "TextBlock.h"
#include "Item.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture2D.h"
#include "StaticMeshResources.h"


bool UItemStackWidget::Initialize() {
	Super::Initialize();

	SetVisibility(ESlateVisibility::HitTestInvisible);

	return true;
}

void UItemStackWidget::Init(FItemStack *_itemStack){
	itemStack = _itemStack;
	RefreshStack();
}

void UItemStackWidget::Init(FItemInfo *itemInfo, int32 amount){
	itemStack = new FItemStack(itemInfo, amount);
	RefreshStack();
}

void UItemStackWidget::RefreshStack(){

	if (!itemImage || !amountText) return;

	if (itemStack && itemStack->IsValid()) {
		
		if (itemStack->amount < 2) {
			amountText->SetText(FText::FromString(""));
		}
		else {
			amountText->SetText(FText::FromString(FString::FromInt(itemStack->amount)));
		}
		
		UMaterialInstance * materialInstance = Cast<UMaterialInstance>(itemStack->itemInfo->texture);

		if (materialInstance) {
			UMaterialInstanceDynamic* dynMaterial = UMaterialInstanceDynamic::Create(materialInstance, this);
			FLinearColor color(1, 1, 1, 1);
			dynMaterial->SetVectorParameterByIndex(0, color);
			//itemStack->itemInfo->texture = dynMaterial;
		}

		if (itemStack->itemInfo->texture) {
			UTexture2D* texture = Cast<UTexture2D>(itemStack->itemInfo->texture);

			if (texture) {
				itemImage->SetBrushFromTexture(texture);
				//itemImage->SetBrushFromTexture(CreateProcTexture(texture)); 
			}
			
		}


		FSlateBrush brush;
		brush.SetResourceObject(itemStack->itemInfo->texture);
		
		//itemImage->SetBrush(brush);
		itemImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));

	}else {
		amountText->SetText(FText::FromString(""));
		itemImage->SetColorAndOpacity(FLinearColor::Transparent);
	}

}



void UItemStackWidget::SetItemStack(FItemStack & _itemStack){
	*itemStack = _itemStack;
}

FItemStack * const UItemStackWidget::GetItemStack() const{ 
	return itemStack;
}

UTexture2D * CreateProcTexture(UTexture2D * texture){

	if (texture->GetFName() == "ColorTest") {
		UE_LOG(LogTemp, Warning, TEXT("TextureName: %s"), *texture->GetFName().ToString());
	}
	
	

	int32 w, h;
	w = texture->GetSizeX();
	h = texture->GetSizeY();
	int32 mDataSize = w * h * 4;
	int32 mDataSqrtSize = w * 4;
	
	uint8* mTextureColors = new uint8[mDataSize];
	uint8* mDynamicColors = new uint8[mDataSize];
	
	FUpdateTextureRegion2D * mUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, w, h);
	
	UTexture2D* dynamicTexture = UTexture2D::CreateTransient(w, h);
	dynamicTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	dynamicTexture->SRGB = 1;
	dynamicTexture->AddToRoot();
	dynamicTexture->UpdateResource();
	
	FTexture2DMipMap& readMip = texture->PlatformData->Mips[0];
	readMip.BulkData.GetCopy((void**)&mTextureColors);
	FMemory::Memcpy(mDynamicColors, mTextureColors, mDataSize);
	
	//UE_LOG(LogTemp, Warning, TEXT("colors: %s"), *FColor(mDynamicColors[0], mDynamicColors[1], mDynamicColors[2], mDynamicColors[3]).ToString());

	
	
	int32 pixelAmount = mDataSize / 4;
	for (int32 i = 0; i < pixelAmount; ++i)
	{
		int32 blue = i * 4 + 0;
		int32 green = i * 4 + 1;
		int32 red = i * 4 + 2;
		int32 alpha = i * 4 + 3;

		//mDynamicColors[blue] = 0;
		//mDynamicColors[green] = 0;
		//mDynamicColors[red] = FMath::Rand(); // Set pixel's red value to 120
		//mDynamicColors[alpha] = 255;
	}
	
	dynamicTexture->UpdateTextureRegions(0, 1, mUpdateTextureRegion, mDataSqrtSize, (uint32)4, mDynamicColors, [](uint8* SrcData, const FUpdateTextureRegion2D* Regions) { delete Regions; FMemory::Free(SrcData); });
	//UpdateTextureRegions(dynamicTexture, 0, 1, mUpdateTextureRegion, mDataSqrtSize, (uint32)4, mDynamicColors, true);
	
	return dynamicTexture;
}

//void UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData){
//	if (Texture->Resource)
//	{
//		struct FUpdateTextureRegionsData
//		{
//			FTexture2DResource* Texture2DResource;
//			int32 MipIndex;
//			uint32 NumRegions;
//			FUpdateTextureRegion2D* Regions;
//			uint32 SrcPitch;
//			uint32 SrcBpp;
//			uint8* SrcData;
//		};
//
//		FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;
//
//		RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
//		RegionData->MipIndex = MipIndex;
//		RegionData->NumRegions = NumRegions;
//		RegionData->Regions = Regions;
//		RegionData->SrcPitch = SrcPitch;
//		RegionData->SrcBpp = SrcBpp;
//		RegionData->SrcData = SrcData;
//
//		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
//			UpdateTextureRegionsData,
//			FUpdateTextureRegionsData*, RegionData, RegionData,
//			bool, bFreeData, bFreeData,
//			{
//				for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
//				{
//					int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
//					if (RegionData->MipIndex >= CurrentFirstMip)
//					{
//						RHIUpdateTexture2D(
//							RegionData->Texture2DResource->GetTexture2DRHI(),
//							RegionData->MipIndex - CurrentFirstMip,
//							RegionData->Regions[RegionIndex],
//							RegionData->SrcPitch,
//							RegionData->SrcData
//							+ RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
//							+ RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
//						);
//					}
//				}
//		if (bFreeData)
//		{
//			FMemory::Free(RegionData->Regions);
//			FMemory::Free(RegionData->SrcData);
//		}
//		delete RegionData;
//			});
//	}
//}
