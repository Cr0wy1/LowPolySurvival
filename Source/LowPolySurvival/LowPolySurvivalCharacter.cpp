// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "LowPolySurvivalCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "PlayerHUDWidget.h"
#include "InventoryWidget.h"
#include "InventoryComponent.h"
#include "InventoryManagerWidget.h"
#include "Components/StaticMeshComponent.h"
#include "AttributeComponent.h"
#include "Logistic.h"
#include "MechArmActor.h"
#include "PlacementComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/WidgetInteractionComponent.h"
#include "PlacementMenuWidget.h"
#include "PlayercharController.h"
#include "CraftingComponent.h"
#include "Chunk.h"
#include "MyGameInstance.h"
#include "WorldGenerator.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CrosshairTraceComponent.h"


DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ALowPolySurvivalCharacter

ALowPolySurvivalCharacter::ALowPolySurvivalCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(30.f, 96.0f);

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


	//Crosshair Trace
	crossTraceComp = CreateDefaultSubobject<UCrosshairTraceComponent>("Crosshair Trace");
	crosshairResult = crossTraceComp->GetCrosshairResultPtr();

	//Inventoriesy
	inventoryComp = CreateDefaultSubobject<UInventoryComponent>("Inventory");
	quickInventoryComp = CreateDefaultSubobject<UInventoryComponent>("Quick Inventory");
	equipmentInventoryComp = CreateDefaultSubobject<UInventoryComponent>("Equipment Inventory");
	

	//Crafting
	craftingComp = CreateDefaultSubobject<UCraftingComponent>("Crafting");
	craftingComp->AddInventoryAccess(inventoryComp, EInvAccess::BOTH);
	craftingComp->AddInventoryAccess(quickInventoryComp, EInvAccess::BOTH);
	craftingComp->AddInventoryAccess(equipmentInventoryComp, EInvAccess::INPUT);
	craftingComp->Init();

	//Attributes
	attributeComp = CreateDefaultSubobject<UAttributeComponent>("Attributes");

	//Placement
	placementComp = CreateDefaultSubobject<UPlacementComponent>("Placement");
	placementComp->Init(this);

	//Place Widget
	placeWidgetComp = CreateDefaultSubobject<UChildActorComponent>("Place Widget");
	placeWidgetComp->SetupAttachment(RootComponent);

	widgetInteractionComp = CreateDefaultSubobject<UWidgetInteractionComponent>("Widget Interaction");
	widgetInteractionComp->SetupAttachment(FirstPersonCameraComponent);

	//Movement
	movementComp = GetCharacterMovement();
	movementComp->JumpZVelocity = 860.0f;
	movementComp->GravityScale = 3.0f;
	movementComp->MaxFlySpeed = 2000.0f;
	movementComp->BrakingDecelerationFlying = 2000.0f;
}



void ALowPolySurvivalCharacter::BeginPlay(){

	Super::BeginPlay();

	gameInstance = GetGameInstance<UMyGameInstance>();

	//UE_LOG(LogTemp, Warning, TEXT("Character: Begin Play"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	Mesh1P->SetHiddenInGame(false, true);

	controller = Cast<APlayercharController>(GetController());
	armActor = Cast<AMechArmActor>(armActorComp->GetChildActor());


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

	if (placementMenu_BP) {
		placementMenuWidget = CreateWidget<UPlacementMenuWidget>(controller, placementMenu_BP);
		placementMenuWidget->AddToViewport();
	}

	
}

void ALowPolySurvivalCharacter::AddItemStackToInventory(UPARAM(ref) FItemStack &itemstack, bool bIsNew){

	bIsNew ? inventoryComp->AddNewStack(itemstack) : inventoryComp->AddStack(itemstack);

}

void ALowPolySurvivalCharacter::OpenInventory(ALogistic* logistic){
	
	if (inventoryManager) {
		inventoryManager->ShowInventory(logistic->inventoryComp);
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

	if (crosshairResult->IsActorHit() ) {
		ABuildings* targetBuilding = crosshairResult->GetHitActor<ABuildings>();
		AChunk* chunk = crosshairResult->GetHitActor<AChunk>();

		if (targetBuilding) {
			playerHUDWidget->UpdateTargetIndicator(targetBuilding->info);
			playerHUDWidget->ShowTargetIndicator();
		}
		else if (chunk) {
			playerHUDWidget->UpdateTargetIndicator(chunk->GetBlock(crosshairResult->hitResult.ImpactPoint));
			playerHUDWidget->ShowTargetIndicator();

			FVector blockCenter = FVector(WorldToBlockLocation(crosshairResult->hitResult.ImpactPoint + crosshairResult->hitDirection)) * 100 + FVector(50,50,50);
			DrawDebugBox(GetWorld(), blockCenter, FVector(50, 50, 50), FColor::Purple, false, -1, 0, 3);
		}
		else {
			playerHUDWidget->HideTargetIndicator();
		}
	}

	controller->UpdateDebugScreen();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ALowPolySurvivalCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{

	UE_LOG(LogTemp, Warning, TEXT("Setup Input Bindings"));


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
	PlayerInputComponent->BindAxis("MoveUp", this, &ALowPolySurvivalCharacter::MoveUp);

	//Inventory
	PlayerInputComponent->BindAction("ToggleInventory", IE_Pressed, this, &ALowPolySurvivalCharacter::ToggleInventory);
	PlayerInputComponent->BindAction("SecondaryAction", IE_Pressed, this, &ALowPolySurvivalCharacter::OnInteraction);
	
	//Placement 
	PlayerInputComponent->BindAction("GridSnapping", IE_Pressed, placementComp, &UPlacementComponent::ToggleGridSnapping);
	PlayerInputComponent->BindAction("ObjectSnapping", IE_Pressed, placementComp, &UPlacementComponent::ToggleObjectSnapping);
	PlayerInputComponent->BindAction("Intersect", IE_Pressed, placementComp, &UPlacementComponent::ToggleIntersect);
	PlayerInputComponent->BindAction("SwitchOrigin", IE_Pressed, placementComp, &UPlacementComponent::SwitchOrigin);

	PlayerInputComponent->BindAction("ScrollDown", IE_Pressed, this, &ALowPolySurvivalCharacter::OnScrollDown);
	PlayerInputComponent->BindAction("ScrollUp", IE_Pressed, this, &ALowPolySurvivalCharacter::OnScrollUp);



	//Alt
	PlayerInputComponent->BindAction("Alt", IE_Pressed, this, &ALowPolySurvivalCharacter::OnAltPressed);
	PlayerInputComponent->BindAction("Alt", IE_Released, this, &ALowPolySurvivalCharacter::OnAltReleased);

	//AltIntersect
	PlayerInputComponent->BindAction("Shift", IE_Pressed, this, &ALowPolySurvivalCharacter::OnShiftPressed);
	PlayerInputComponent->BindAction("Shift", IE_Released, this, &ALowPolySurvivalCharacter::OnShiftReleased);

	//OpenPlacementMenu
	PlayerInputComponent->BindAction("OpenPlacementMenu", IE_Pressed, this, &ALowPolySurvivalCharacter::OnOpenPlacementMenuPressed);
	PlayerInputComponent->BindAction("OpenPlacementMenu", IE_Released, this, &ALowPolySurvivalCharacter::OnOpenPlacementMenuReleased);

	//R
	PlayerInputComponent->BindAction("R", IE_Pressed, placementComp, &UPlacementComponent::OnRPressed);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ALowPolySurvivalCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ALowPolySurvivalCharacter::LookUpAtRate);
}

void ALowPolySurvivalCharacter::OnHit(){

	FVector dir;
	float length;
	(crosshairResult->hitResult.ImpactPoint - GetActorLocation()).ToDirectionAndLength(dir, length);

	if (length <= attributeComp->GetAttributes().hitRange) {


		ABuildings* building = crosshairResult->GetHitActor<ABuildings>();

		if (building) {
			//UE_LOG(LogTemp, Warning, TEXT("%s"), *hitResult.GetActor()->GetName());

			int32 damage = 10;
			
			if (rightHandStack && rightHandStack->IsValid()) {
				damage += rightHandStack->itemInfo->damage;
			}

			building->ApplyDamage(damage, this);
		}


		AChunk* chunk = crosshairResult->GetHitActor<AChunk>();

		if (chunk) {
			
			FIntVector blockLoc = WorldToBlockLocation(crosshairResult->hitResult.ImpactPoint + crosshairResult->hitDirection);

			gameInstance->GetWorldGenerator()->HitBlock(FIntVector(blockLoc), 100, this);
			//gameInstance->GetWorldGenerator()->HitBlock(FIntVector(blockLoc) + FIntVector(0,0,1), 100, this);
			//gameInstance->GetWorldGenerator()->HitBlock(FIntVector(blockLoc) + FIntVector(0, 1, 0), 100, this);
			//gameInstance->GetWorldGenerator()->HitBlock(FIntVector(blockLoc) + FIntVector(1, 0, 0), 100, this);
			//gameInstance->GetWorldGenerator()->HitBlock(FIntVector(blockLoc) + FIntVector(0, 0, -1), 100, this);
			//gameInstance->GetWorldGenerator()->HitBlock(FIntVector(blockLoc) + FIntVector(0, -1, 0), 100, this);  
			//gameInstance->GetWorldGenerator()->HitBlock(FIntVector(blockLoc) + FIntVector(-1, 0, 0), 100, this);
			
		}

		bIsInHit = true;

		ApplyDamage(1);

	}

	
}

void ALowPolySurvivalCharacter::OnPrimaryPressed(){
	bIsHoldingPrimary = true;
	

	if (placementComp->IsPlacementActive()) {
		
		if (placementComp->PlaceBuilding()) {
			int32 selecQSlot = playerHUDWidget->GetCurrentSelectedQuickSlot();
			quickInventoryComp->RemoveStack(selecQSlot);

			OnUpdateHandStack();
		}
	}
	craftingComp->Craft("1", 2);
}

void ALowPolySurvivalCharacter::OnPrimaryReleased(){
	bIsHoldingPrimary = false;
	Mesh1P->GetAnimInstance()->StopSlotAnimation();
}

void ALowPolySurvivalCharacter::OnAltPressed(){
	bIsHoldingAlt = true;
}

void ALowPolySurvivalCharacter::OnAltReleased(){
	bIsHoldingAlt = false;
}

void ALowPolySurvivalCharacter::OnShiftPressed(){

	movementComp->MaxWalkSpeed = runSpeed;

	bIsHoldingShift = true;
}

void ALowPolySurvivalCharacter::OnShiftReleased(){

	movementComp->MaxWalkSpeed = walkSpeed;

	bIsHoldingShift = false;
}

void ALowPolySurvivalCharacter::ToggleInventory(){

	inventoryManager->ShowInventory(inventoryComp);
}

void ALowPolySurvivalCharacter::OnInteraction(){

	ABuildings* building = crosshairResult->GetHitActor<ABuildings>();

	if (building) {
		currentInteractionBuilding = building;
		building->Interact(this);
	}
	else {
		AChunk* chunk = crosshairResult->GetHitActor<AChunk>();

		if (chunk) {
			FIntVector blockLoc = WorldToBlockLocation(crosshairResult->hitResult.ImpactPoint - (crosshairResult->hitDirection * 25));

			gameInstance->GetWorldGenerator()->SetBlock(FIntVector(blockLoc), FBlock::FromId(this, 8));

			DrawDebugPoint(GetWorld(), crosshairResult->hitResult.ImpactPoint, 5, FColor::Red, false, 30);
			DrawDebugBox(GetWorld(), FVector(blockLoc) * 100, FVector(10, 10, 10), FColor::White, false, 60, 0, 1);

		}
	}
}

void ALowPolySurvivalCharacter::OnScrollDown(){

	if (bIsHoldingAlt) {
		placementComp->AddPlaceRotation(-10, 0);
	}
	else if (bIsHoldingShift) {
		placementComp->AddPlaceRotation(0, -10);
	}
	else {
		rightHandStack = playerHUDWidget->OnScrollDown();

	}


	OnScroll();
}

void ALowPolySurvivalCharacter::OnScrollUp(){

	if (bIsHoldingAlt) {
		placementComp->AddPlaceRotation(10, 0);
	}
	else if (bIsHoldingShift) {
		placementComp->AddPlaceRotation(0, 10);
	}
	else {
		rightHandStack = playerHUDWidget->OnScrollUp();
	}

	OnScroll();
}

void ALowPolySurvivalCharacter::OnScroll(){

	if (!bIsHoldingAlt && !bIsHoldingShift) {
		OnUpdateHandStack();
	}
	

}

void ALowPolySurvivalCharacter::OnOpenPlacementMenuPressed(){
	if (placementMenuWidget) {
		placementMenuWidget->OpenUI();
	}
}

void ALowPolySurvivalCharacter::OnOpenPlacementMenuReleased(){
	if (placementMenuWidget) {
		//placementMenuWidget->CloseUI();
	}
}

void ALowPolySurvivalCharacter::OnInventoryOpen(){

}

void ALowPolySurvivalCharacter::OnInventoryClose(){
	if (currentInteractionBuilding) {
		currentInteractionBuilding->OnInteractEnd();
		currentInteractionBuilding = nullptr;
	}
}

void ALowPolySurvivalCharacter::OnUpdateHandStack(){

	placementComp->DeactivatePlacement();

	if (rightHandStack && rightHandStack->IsValid()) {
		if (rightHandStack->itemInfo->buildingTemplate_BP) {
			if (rightHandStack->itemInfo->bCanPlace) {
				placementComp->ActivatePlacement(rightHandStack->itemInfo);
			}
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

const FCrosshairResult * ALowPolySurvivalCharacter::GetCrosshairResultPtr() const{
	return crossTraceComp->GetCrosshairResultPtr();
}

void ALowPolySurvivalCharacter::UpdateMeshRightHand(){

	meshRightHand->SetVisibility(false);
	
	if (rightHandStack && rightHandStack->IsValid() && rightHandStack->itemInfo->bCanHold) {

		if (rightHandStack->itemInfo->buildingTemplate_BP) {
			ABuildings* itemBuilding = rightHandStack->itemInfo->buildingTemplate_BP->GetDefaultObject<ABuildings>();
			armActor->SetInHandMesh(itemBuilding->GetStaticMesh());
		}
		else if (rightHandStack->itemInfo->mesh) {
			armActor->SetInHandMesh(rightHandStack->itemInfo->mesh);
		}
		else {
			armActor->RemoveInHandMesh();
		}

		
	}else {
		//UE_LOG(LogTemp, Warning, TEXT("UpdateMeshRightHand: itemBuidling not found!"));
		armActor->RemoveInHandMesh();
	}

}

void ALowPolySurvivalCharacter::SetPlayerMode(EPlayerMode _playerMode){
	if (playerMode != _playerMode) {
		playerMode = _playerMode;

		switch (playerMode){

			case EPlayerMode::CREATIVE:
				movementComp->SetMovementMode(EMovementMode::MOVE_Flying);
				break;
			case EPlayerMode::SPECTATOR:
				movementComp->SetMovementMode(EMovementMode::MOVE_Flying);
				break;
			default: //default = SURVIVAL
				movementComp->SetMovementMode(EMovementMode::MOVE_Walking);
				break;
			}
	}
}


void ALowPolySurvivalCharacter::MoveForward(float Value){

	if (Value != 0.0f){

		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ALowPolySurvivalCharacter::MoveRight(float Value){

	if (Value != 0.0f){

		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ALowPolySurvivalCharacter::MoveUp(float Value){
	if (Value != 0.0f) {

		// add movement in that direction
		AddMovementInput(GetActorUpVector(), Value);
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


