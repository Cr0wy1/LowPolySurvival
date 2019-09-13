// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayercharController.h"
#include "DebugWidget.h"
#include "MyGameInstance.h"
#include "WorldGenerator.h"
#include "EscapeMenuWidget.h"
#include "WidgetAsset.h" 
#include "UChatWidget.h"
#include "MapWidget.h"


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

	if (widgetAsset_A->chatWidget_BP) {
		chatWidget = CreateWidget<UChatWidget>(this, widgetAsset_A->chatWidget_BP);
		chatWidget->AddToPlayerScreen();
	}

	if (widgetAsset_A->mapWidget_BP) {
		mapWidget = CreateWidget<UMapWidget>(this, widgetAsset_A->mapWidget_BP);
		mapWidget->AddToPlayerScreen();
	}
}

void APlayercharController::SetupInputComponent() {

	Super::SetupInputComponent();

	InputComponent->BindAction("ToggleDebugScreen", IE_Pressed, this, &APlayercharController::OnToggleDebugScreen);
	InputComponent->BindAction("EscapeMenu", IE_Pressed, this, &APlayercharController::OnEscapeMenuPressed);
	InputComponent->BindAction("OpenChat", IE_Pressed, this, &APlayercharController::OnOpenChatPressed);
	InputComponent->BindAction("OpenMap", IE_Pressed, this, &APlayercharController::OnOpenMapPressed);

	

}



void APlayercharController::OnToggleDebugScreen(){
	bIsDebugScreenShown = !bIsDebugScreenShown;

	if (debugWidget) {
		bIsDebugScreenShown ? debugWidget->SetVisibility(ESlateVisibility::HitTestInvisible) : debugWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	//UE_LOG(LogTemp, Warning, TEXT("DebugScreenShown: %i"), bIsDebugScreenShown);

}

void APlayercharController::OnEscapeMenuPressed(){
	if (escapeMenuWidget) {
		escapeMenuWidget->OpenUI();
	}

	//UE_LOG(LogTemp, Warning, TEXT("EscapeMenu"), bIsDebugScreenShown);
}

void APlayercharController::OnOpenChatPressed(){
	if (chatWidget) {
		chatWidget->OpenUI();
	}


}

void APlayercharController::OnOpenMapPressed(){
	if (mapWidget) {
		mapWidget->OpenUI();
		mapWidget->RenderMap();
	}
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
