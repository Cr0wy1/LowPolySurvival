// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "LowPolySurvivalCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Buildings.h"
#include "PlayerHUDWidget.h"
#include "InventoryWidget.h"
#include "InventoryComponent.h"
#include "InventoryManagerWidget.h"
#include "Components/StaticMeshComponent.h"
#include "AttributeComponent.h"
#include "Construction.h"
#include "MechArmActor.h"
#include "PlacementComponent.h"
#include "Animation/AnimInstance.h"

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

	armActorComp = CreateDefaultSubobject<UChildActorComponent>("Arm Actor");
	armActorComp->SetupAttachment(Mesh1P, FName("arm_socket"));
	
	

	sceneRightHand = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Right Hand"));
	sceneRightHand->SetupAttachment(Mesh1P, FName("tool_socket"));

	meshRightHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Right Hand"));
	meshRightHand->SetupAttachment(sceneRightHand);

	skeletalMeshRightHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh Right Hand"));
	skeletalMeshRightHand->SetupAttachment(sceneRightHand);
	

	//Inventoriesy
	inventoryComp = CreateDefaultSubobject<UInventoryComponent>("Inventory");
	quickInventoryComp = CreateDefaultSubobject<UInventoryComponent>("Quick Inventory");
	equipmentInventoryComp = CreateDefaultSubobject<UInventoryComponent>("Equipment Inventory");
	

	//Attributes
	attributeComp = CreateDefaultSubobject<UAttributeComponent>("Attributes");

	//Placement
	placementComp = CreateDefaultSubobject<UPlacementComponent>("Placement");
	placementComp->Init(this);
}



void ALowPolySurvivalCharacter::BeginPlay(){

	Super::BeginPlay();

	//UE_LOG(LogTemp, Warning, TEXT("Character: Begin Play"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	Mesh1P->SetHiddenInGame(false, true);

	controller = Cast<APlayerController>(GetController());
	armActor = Cast<AMechArmActor>(armActorComp->GetChildActor());

	if (armActor) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *armActor->GetName());

	}

	controller->GetViewportSize(viewX, viewY);
	//CreateWidget(controller, )

	sceneRightHand->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false), FName("hand_R_tool"));


	if (inventoryManager_BP && playerHUDWidget_BP) {
		inventoryManager = CreateWidget<UInventoryManagerWidget>(controller, inventoryManager_BP);
		playerHUDWidget = CreateWidget<UPlayerHUDWidget>(controller, playerHUDWidget_BP);

		inventoryManager->Init(this, playerHUDWidget->playerQuickInv);
		playerHUDWidget->Init(&attributeComp->GetAttributesRef(), equipmentInventoryComp);

		playerHUDWidget->BindQuickSlot(quickInventoryComp, inventoryManager);

		playerHUDWidget->AddToViewport();
		inventoryManager->AddToViewport();
	}

	
}

void ALowPolySurvivalCharacter::AddItemStackToInventory(FItemStack &itemstack){

	inventoryComp->AddStack(itemstack);

}

void ALowPolySurvivalCharacter::OpenInventory(AConstruction* construction){
	//UE_LOG(LogTemp, Warning, TEXT("Open Inventory"));
	if (inventoryManager) {
		inventoryManager->ShowInventory(construction->inventoryComp);
	}
	
}

void ALowPolySurvivalCharacter::ApplyDamage(int32 amount, AActor * causer){

	attributeComp->RemoveHealth(amount);

	playerHUDWidget->UpdateHealth();
	
}

APlayerController * ALowPolySurvivalCharacter::GetPlayerController() const{
	return controller;
}

void ALowPolySurvivalCharacter::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	
}

//////////////////////////////////////////////////////////////////////////
// Input

void ALowPolySurvivalCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{

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

	PlayerInputComponent->BindAction("ScrollDown", IE_Pressed, this, &ALowPolySurvivalCharacter::OnScrollDown);
	PlayerInputComponent->BindAction("ScrollUp", IE_Pressed, this, &ALowPolySurvivalCharacter::OnScrollUp);

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

	FVector dir;
	float length;
	(hitResult.ImpactPoint - GetActorLocation()).ToDirectionAndLength(dir, length);

	//UE_LOG(LogTemp, Warning, TEXT("%f"), length);
	 

	if (length <= attributeComp->GetAttributes().hitRange) {


		ABuildings* building = Cast<ABuildings>(hitResult.GetActor());

		if (building) {
			UE_LOG(LogTemp, Warning, TEXT("%s"), *hitResult.GetActor()->GetName());

			building->ApplyDamage(10, this);
		}


		bIsInHit = true;

		ApplyDamage(1);

	}

	
}

FHitResult ALowPolySurvivalCharacter::CrosshairLineTrace(){

	FVector worldLocation, worldDirection;
	controller->DeprojectScreenPositionToWorld(viewX * 0.5f, viewY * 0.5f, worldLocation, worldDirection);

	FVector startLocation = worldDirection * 100 + worldLocation;
	FVector endLocation = worldDirection * 1000 + startLocation;

	FHitResult hitResult;
	GetWorld()->LineTraceSingleByChannel(hitResult, startLocation, endLocation, ECollisionChannel::ECC_GameTraceChannel2);

	if (hitResult.GetActor()) {
		//DrawDebugLine(GetWorld(), startLocation, hitResult.ImpactPoint, FColor::Red, false, -1.0f, 0, 1.0f);

	}

	return hitResult;
}

void ALowPolySurvivalCharacter::OnPrimaryPressed(){
	bIsHoldingPrimary = true;
	

	if (placementComp->IsPlacementActive()) {
		
		if (placementComp->PlaceBuilding()) {
			int32 selecQSlot = playerHUDWidget->GetCurrentSelectedQuickSlot();
			quickInventoryComp->TakeOffFromSlot(selecQSlot);

			OnUpdateHandStack();
		}
	}

}

void ALowPolySurvivalCharacter::OnPrimaryReleased(){
	bIsHoldingPrimary = false;
	Mesh1P->GetAnimInstance()->StopSlotAnimation();
}

void ALowPolySurvivalCharacter::ToggleInventory(){
	//inventory->AddToPlayerViewport(controller);
	//UE_LOG(LogTemp, Warning, TEXT("Toggle Inventory"));

	inventoryManager->ShowInventory(inventoryComp);
}

void ALowPolySurvivalCharacter::OnInteraction(){
	FHitResult hitResult = CrosshairLineTrace();

	ABuildings* building = Cast<ABuildings>(hitResult.GetActor());

	if (building) {
		currentInteractionBuilding = building;
		building->Interact(this);
	}
	
}

void ALowPolySurvivalCharacter::OnScrollDown(){

	rightHandStack = playerHUDWidget->OnScrollDown();
	
	OnScroll();
}

void ALowPolySurvivalCharacter::OnScrollUp(){

	rightHandStack = playerHUDWidget->OnScrollUp();

	OnScroll();
}

void ALowPolySurvivalCharacter::OnScroll(){

	OnUpdateHandStack();

}

void ALowPolySurvivalCharacter::OnInventoryOpen()
{
}

void ALowPolySurvivalCharacter::OnInventoryClose(){
	if (currentInteractionBuilding) {
		currentInteractionBuilding->OnInteractEnd();
		currentInteractionBuilding = nullptr;
	}
}

void ALowPolySurvivalCharacter::OnUpdateHandStack(){

	placementComp->DeactivatePlacement();

	if (rightHandStack && rightHandStack->IsValid() && rightHandStack->itemInfo->building_BP) {
		if (rightHandStack->itemInfo->type != EItemType::TOOL) {
			placementComp->ActivatePlacement(rightHandStack->itemInfo->building_BP);
		}
	}

	UpdateMeshRightHand();
}

bool ALowPolySurvivalCharacter::GetIsInHitAnimation() const{

	return bIsInHit;
}

void ALowPolySurvivalCharacter::SetIsInHitAnimationb(bool b){
	bIsInHit = b;

	
}

AMechArmActor * ALowPolySurvivalCharacter::GetArmActor() const{
	return armActor;
}

void ALowPolySurvivalCharacter::UpdateMeshRightHand(){

	meshRightHand->SetVisibility(false);
	skeletalMeshRightHand->SetVisibility(false);
	
	if (rightHandStack && rightHandStack->IsValid() && rightHandStack->itemInfo->building_BP) {

		ABuildings* itemBuilding = rightHandStack->itemInfo->building_BP->GetDefaultObject<ABuildings>();


		if (itemBuilding->IsSkeletalMesh()) {

			if (skeletalMeshRightHand->SkeletalMesh != itemBuilding->GetSkeletalMesh()) {
				skeletalMeshRightHand->SetSkeletalMesh(itemBuilding->GetSkeletalMesh());

			}

			skeletalMeshRightHand->SetVisibility(true);

		}
		else {

			if (rightHandStack->itemInfo->type == EItemType::TOOL) {
				armActor->SetInHandMesh(itemBuilding->GetStaticMesh());
			}
			else {
				armActor->RemoveInHandMesh();
			}
				
			
		}
		

		

	}
	else {
		//UE_LOG(LogTemp, Warning, TEXT("UpdateMeshRightHand: itemBuidling not found!"));
		armActor->RemoveInHandMesh();
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


