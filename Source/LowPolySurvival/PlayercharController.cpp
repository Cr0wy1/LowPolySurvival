// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayercharController.h"




void APlayercharController::CenterMouse() {
	int32 viewX, viewY;
	GetViewportSize(viewX, viewY);

	SetMouseLocation(viewX*0.5f, viewY*0.5f);
}