// Fill out your copyright notice in the Description page of Project Settings.


#include "RuntimeDrawingLibrary.h"
#include "Components/LineBatchComponent.h"

void URuntimeDrawingLibrary::FlushPersistentLines(const UWorld* InWorld){
	if (InWorld && InWorld->PersistentLineBatcher){
		InWorld->PersistentLineBatcher->Flush();
	}
}

ULineBatchComponent* GetLineBatcher(const UWorld* InWorld, bool bPersistentLines, float LifeTime, bool bDepthIsForeground){
	return (InWorld ? (bDepthIsForeground ? InWorld->ForegroundLineBatcher : ((bPersistentLines || (LifeTime > 0.f)) ? InWorld->PersistentLineBatcher : InWorld->LineBatcher)) : NULL);
}

static float GetLineLifeTime(ULineBatchComponent* LineBatcher, float LifeTime, bool bPersistent){
	return bPersistent ? -1.0f : ((LifeTime > 0.f) ? LifeTime : LineBatcher->DefaultLifeTime);
}

void URuntimeDrawingLibrary::DrawBox(const UWorld* InWorld, FVector const& Center, FVector const& Box, FColor const& Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority, float Thickness) {

	// this means foreground lines can't be persistent 
	ULineBatchComponent* const LineBatcher = GetLineBatcher(InWorld, bPersistentLines, LifeTime, (DepthPriority == SDPG_Foreground));
	if (LineBatcher != NULL)
	{
		float LineLifeTime = GetLineLifeTime(LineBatcher, LifeTime, bPersistentLines);

		LineBatcher->DrawLine(Center + FVector(Box.X, Box.Y, Box.Z), Center + FVector(Box.X, -Box.Y, Box.Z), Color, DepthPriority, Thickness, LineLifeTime);
		LineBatcher->DrawLine(Center + FVector(Box.X, -Box.Y, Box.Z), Center + FVector(-Box.X, -Box.Y, Box.Z), Color, DepthPriority, Thickness, LineLifeTime);
		LineBatcher->DrawLine(Center + FVector(-Box.X, -Box.Y, Box.Z), Center + FVector(-Box.X, Box.Y, Box.Z), Color, DepthPriority, Thickness, LineLifeTime);
		LineBatcher->DrawLine(Center + FVector(-Box.X, Box.Y, Box.Z), Center + FVector(Box.X, Box.Y, Box.Z), Color, DepthPriority, Thickness, LineLifeTime);

		LineBatcher->DrawLine(Center + FVector(Box.X, Box.Y, -Box.Z), Center + FVector(Box.X, -Box.Y, -Box.Z), Color, DepthPriority, Thickness, LineLifeTime);
		LineBatcher->DrawLine(Center + FVector(Box.X, -Box.Y, -Box.Z), Center + FVector(-Box.X, -Box.Y, -Box.Z), Color, DepthPriority, Thickness, LineLifeTime);
		LineBatcher->DrawLine(Center + FVector(-Box.X, -Box.Y, -Box.Z), Center + FVector(-Box.X, Box.Y, -Box.Z), Color, DepthPriority, Thickness, LineLifeTime);
		LineBatcher->DrawLine(Center + FVector(-Box.X, Box.Y, -Box.Z), Center + FVector(Box.X, Box.Y, -Box.Z), Color, DepthPriority, Thickness, LineLifeTime);

		LineBatcher->DrawLine(Center + FVector(Box.X, Box.Y, Box.Z), Center + FVector(Box.X, Box.Y, -Box.Z), Color, DepthPriority, Thickness, LineLifeTime);
		LineBatcher->DrawLine(Center + FVector(Box.X, -Box.Y, Box.Z), Center + FVector(Box.X, -Box.Y, -Box.Z), Color, DepthPriority, Thickness, LineLifeTime);
		LineBatcher->DrawLine(Center + FVector(-Box.X, -Box.Y, Box.Z), Center + FVector(-Box.X, -Box.Y, -Box.Z), Color, DepthPriority, Thickness, LineLifeTime);
		LineBatcher->DrawLine(Center + FVector(-Box.X, Box.Y, Box.Z), Center + FVector(-Box.X, Box.Y, -Box.Z), Color, DepthPriority, Thickness, LineLifeTime);
	}
}