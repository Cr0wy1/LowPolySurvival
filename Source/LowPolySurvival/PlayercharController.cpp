// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayercharController.h"
#include "DebugWidget.h"
#include "MyGameInstance.h"
#include "WorldGenerator.h"
#include "EscapeMenuWidget.h"
#include "WidgetAsset.h" 


void APlayercharController::BeginPlay() {
	Super::BeginPlay();

	gameInstance = GetGameInstance<UMyGameInstance>();
	UWidgetAsset* widgetAsset_A = gameInstance->GetWidgetAsset();

	if (widgetAsset_A->debugWidget_BP) {
		debugWidget = CreateWidget<UDebugWidget>(this, widgetAsset_A->debugWidget_BP);
		debugWidget->AddToPlayerScreen();
	}

	if (widgetAsset_A->escapeMenuWidget_BP) {
		escapeMenuWidget = CreateWidget<UEscapeMenuWidget>(this, widgetAsset_A->escapeMenuWidget_BP);
		escapeMenuWidget->AddToPlayerScreen();
	}

}

void APlayercharController::SetupInputComponent() {

	Super::SetupInputComponent();

	InputComponent->BindAction("ToggleDebugScreen", IE_Pressed, this, &APlayercharController::OnToggleDebugScreen);
	InputComponent->BindAction("EscapeMenu", IE_Pressed, this, &APlayercharController::OnEscapeMenuPressed);

}



void APlayercharController::OnToggleDebugScreen(){
	bIsDebugScreenShown = !bIsDebugScreenShown;

	if (debugWidget) {
		bIsDebugScreenShown ? debugWidget->SetVisibility(ESlateVisibility::HitTestInvisible) : debugWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	UE_LOG(LogTemp, Warning, TEXT("DebugScreenShown: %i"), bIsDebugScreenShown);

}

void APlayercharController::OnEscapeMenuPressed(){
	if (escapeMenuWidget) {
		escapeMenuWidget->OpenUI();
	}

	UE_LOG(LogTemp, Warning, TEXT("EscapeMenu"), bIsDebugScreenShown);
}

void APlayercharController::CenterMouse() {
	int32 viewX, viewY;
	GetViewportSize(viewX, viewY);

	SetMouseLocation(viewX*0.5f, viewY*0.5f);
}

void APlayercharController::UpdateDebugScreen(){

	FDebugInfo debugInfo;
	debugInfo.playerLocation = GetPawn()->GetActorLocation();
	debugInfo.playerVelocity = GetPawn()->GetVelocity();
	debugInfo.playerLookDirection = GetControlRotation();
	debugInfo.loadedChunks = gameInstance->GetWorldGenerator()->GetLoadedChunksNum();

	FBiomeData* biomeData = gameInstance->GetWorldGenerator()->GetBiome(WorldToBlockLocation(debugInfo.playerLocation));

	if (biomeData) {
		debugInfo.biomeName = biomeData->name; 
	}
	

	if (debugWidget) {
		debugWidget->Update(debugInfo);
	}
}
