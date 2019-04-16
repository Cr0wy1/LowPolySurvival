// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "LowPolySurvivalCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/DecalComponent.h"
#include "Buildings.h"
#include "PlayerHUDWidget.h"
#include "InventoryWidget.h"
#include "InventoryComponent.h"
#include "InventoryManagerWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ALowPolySurvivalCharacter

ALowPolySurvivalCharacter::ALowPolySurvivalCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	//Inventory
	inventory = CreateDefaultSubobject<UInventoryComponent>("Inventory");
	
	
}



void ALowPolySurvivalCharacter::BeginPlay(){

	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Character: Begin Play"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	Mesh1P->SetHiddenInGame(false, true);

	controller = Cast<APlayerController>(GetController());

	controller->GetViewportSize(viewX, viewY);
	//CreateWidget(controller, )

	if (playerHUDWidget_BP) {
		playerHUDWidget = CreateWidget<UPlayerHUDWidget>(controller, playerHUDWidget_BP);
		playerHUDWidget->AddToViewport();
	}

	if (inventoryManager_BP) {
		inventoryManager = CreateWidget<UInventoryManagerWidget>(controller, inventoryManager_BP);
		inventoryManager->Init(inventory);
		inventoryManager->AddToViewport();
	}
}

void ALowPolySurvivalCharacter::AddItemStackToInventory(FItemStack &itemstack){

	inventory->AddStack(itemstack);

}

void ALowPolySurvivalCharacter::OpenInventory(UInventoryComponent* inventoryComp){
	UE_LOG(LogTemp, Warning, TEXT("%s"), "Hallo");
	if (inventoryManager) {
		inventoryManager->ShowInventory(inventoryComp);
	}
	
}

APlayerController * ALowPolySurvivalCharacter::GetPlayerController() const{
	return controller;
}

void ALowPolySurvivalCharacter::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	if (controller) {
		//UE_LOG(LogTemp, Warning, TEXT("Ticking"));
		
		CrosshairLineTrace();
	}

	if (bIsHoldingPrimary) {

		if (GetWorld()->GetTimeSeconds() >= nextHitSeconds) {
			OnHit();

			nextHitSeconds = GetWorld()->GetTimeSeconds() + primaryHitCooldown;
		}

		
	}
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void ALowPolySurvivalCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("Setup Input Compoennt"));

	// set up gameplay key bindings
	check(PlayerInputComponent);
	

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &ALowPolySurvivalCharacter::OnPrimaryPressed);
	PlayerInputComponent->BindAction("PrimaryAction", IE_Released, this, &ALowPolySurvivalCharacter::OnPrimaryReleased);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ALowPolySurvivalCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALowPolySurvivalCharacter::MoveRight);

	//Inventory
	PlayerInputComponent->BindAction("ToggleInventory", IE_Pressed, this, &ALowPolySurvivalCharacter::ToggleInventory);
	PlayerInputComponent->BindAction("SecondaryAction", IE_Pressed, this, &ALowPolySurvivalCharacter::OnInteraction);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ALowPolySurvivalCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ALowPolySurvivalCharacter::LookUpAtRate);
}

void ALowPolySurvivalCharacter::OnHit(){

	FHitResult hitResult = CrosshairLineTrace();

	ABuildings* building = Cast<ABuildings>(hitResult.GetActor());

	if (building) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *hitResult.GetActor()->GetName() );

		building->ApplyDamage(10, this);
	}

	
}

FHitResult ALowPolySurvivalCharacter::CrosshairLineTrace(){

	FVector worldLocation, worldDirection;
	controller->DeprojectScreenPositionToWorld(viewX * 0.5f, viewY * 0.5f, worldLocation, worldDirection);

	FVector startLocation = worldDirection * 100 + worldLocation;
	FVector endLocation = worldDirection * 1000 + startLocation;

	FHitResult hitResult;
	GetWorld()->LineTraceSingleByChannel(hitResult, startLocation, endLocation, ECollisionChannel::ECC_WorldStatic);

	if (hitResult.GetActor()) {
		DrawDebugLine(GetWorld(), startLocation, hitResult.ImpactPoint, FColor::Red, false, -1.0f, 0, 1.0f);

	}

	return hitResult;
}

void ALowPolySurvivalCharacter::OnPrimaryPressed(){
	bIsHoldingPrimary = true;
}

void ALowPolySurvivalCharacter::OnPrimaryReleased(){
	bIsHoldingPrimary = false;
}

void ALowPolySurvivalCharacter::ToggleInventory(){
	//inventory->AddToPlayerViewport(controller);
	inventoryManager->ShowInventory(inventory);
}

void ALowPolySurvivalCharacter::OnInteraction(){
	FHitResult hitResult = CrosshairLineTrace();

	ABuildings* building = Cast<ABuildings>(hitResult.GetActor());

	if (building) {
		building->Interact(this);
	}
	
}


void ALowPolySurvivalCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ALowPolySurvivalCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ALowPolySurvivalCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ALowPolySurvivalCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


