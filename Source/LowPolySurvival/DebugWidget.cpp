// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugWidget.h"
#include "TextBlock.h"
#include "MyGameInstance.h"

void UDebugWidget::Update(const FDebugInfo & debugInfo){

	FText playerLoc = FText::FromString("Loc: " + debugInfo.playerLocation.ToCompactString());
	FText playerBlockLoc = FText::FromString("BlockLoc: " + WorldToBlockLocation(debugInfo.playerLocation).ToString());
	FText playerChunkLoc = FText::FromString("ChunkLoc: " + WorldToChunkLocation(debugInfo.playerLocation).ToString());
	FText loadedChunks = FText::FromString("LoadedChunks: " + FString::FromInt(debugInfo.loadedChunks) );
	FText loadedBlocks = FText::FromString("LoadedBlocks: " + FString::FromInt(debugInfo.loadedChunks * FMath::Pow(FWorldParams::chunkSize, 3) ));

	playerLocText->SetText(playerLoc);
	playerBlockLocText->SetText(playerBlockLoc);
	playerChunkLocText->SetText(playerChunkLoc);

	loadedChunksText->SetText(loadedChunks);
	loadedBlocksText->SetText(loadedBlocks);

}
