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
	FText playerVel = FText::FromString("Velocity: " + debugInfo.playerVelocity.ToString() );
	FText playerLookDir = FText::FromString("Look Direction: " + GetCompassFromRotation(debugInfo.playerLookDirection.Yaw) + " (" + debugInfo.playerLookDirection.ToString() + ")");
	FText biome = FText::FromString("Biome: " + debugInfo.biomeName.ToString());
	

	playerLocText->SetText(playerLoc);
	playerBlockLocText->SetText(playerBlockLoc);
	playerChunkLocText->SetText(playerChunkLoc);

	loadedChunksText->SetText(loadedChunks);
	loadedBlocksText->SetText(loadedBlocks);

	playerVelText->SetText(playerVel);
	playerLookDirText->SetText(playerLookDir);

	biomeText->SetText(biome);

}

FString UDebugWidget::GetCompassFromRotation(const float yaw) const{

	FString result;

	if (yaw >= 315 || yaw <= 45) {
		result = "N";
	}
	else if(yaw < 135){
		result = "O";
	}
	else if (yaw < 225) {
		result = "S";
	}
	else {
		result = "W";
	}

	if (yaw > 22.5f && yaw < 67.5f) {
		result = "NO";
	}
	else if (yaw > 112.5f && yaw < 157.5f) {
		result = "SO";
	}
	else if (yaw > 202.5f && yaw < 247.5f) {
		result = "SW";
	}
	else if (yaw > 292.5f && yaw < 337.5f) {
		result = "NW";
	}

	return result;
}
