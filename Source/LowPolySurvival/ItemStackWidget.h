// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "ItemStackWidget.generated.h"


class UImage;
class UTextBlock;
struct FItemStack;
struct FItemData;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UItemStackWidget : public UHUDWidget
{
	GENERATED_BODY()

protected:

	FItemStack * itemStack;

	
public:

	

	virtual bool  Initialize() override;

	

	UPROPERTY(meta = (BindWidget))
	UImage* itemImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* amountText;

	void Init(FItemStack* _itemStack);
	void Init(FItemData* itemInfo, int32 amount = 1);
	void RefreshStack();

	void SetItemStack(FItemStack &_itemStack);
	FItemStack*const GetItemStack()const;

	
	
};

UTexture2D* CreateProcTexture(UTexture2D* texture);
void UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);
