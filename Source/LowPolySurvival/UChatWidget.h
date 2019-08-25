// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIWidget.h"
#include "UChatWidget.generated.h"



class UTextBlock;
class UEditableTextBox;
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UChatWidget : public UUIWidget
{
	GENERATED_BODY()
	

public:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* chatLogText;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* chatInputTextBox;

	virtual bool  Initialize() override;

	FReply NativeOnPreviewKeyDown(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent) override;

	virtual void OpenUI() override;

protected:

	UFUNCTION()
	void ProcessChatInput(const FText& text, ETextCommit::Type commitMethod);

	void ProcessArgs(const TArray<FString> &args);
};
