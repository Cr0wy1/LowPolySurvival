// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugWidget.h"
#include "TextBlock.h"
#include "MyGameInstance.h"

void UDebugWidget::Update(const FDebugInfo & debugInfo){

	FText playerLoc = FText::FromString("Loc: " + debugInfo.playerLocation.ToCompactString());
	FText playerBlockLoc = FText::FromString("BlockLoc: " + WorldToBlockLocation(debugInfo.playerLocation).ToString());
	FText playerChunkLoc = FText::FromString("ChunkLoc: " + WorldToChunkLocation(debugInfo.playerLocation).ToString());

	playerLocText->SetText(playerLoc);
	playerBlockLocText->SetText(playerBlockLoc);
	playerChunkLocText->SetText(playerChunkLoc);

}
