// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	bool bIsUIOpen = false;

	
public:

	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable, Category = "UIWidget")
	void CloseUI();

	virtual void OpenUI();

	bool IsUIOpen() const;

	virtual FReply NativeOnPreviewKeyDown(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent) override;
};
