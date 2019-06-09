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
}



void ALowPolySurvivalCharacter::BeginPlay(){

	Super::BeginPlay();

	//UE_LOG(LogTemp, Warning, TEXT("Character: Begin Play"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	Mesh1P->SetHiddenInGame(false, true);

	controller = Cast<APlayercharController>(GetController());
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

	if (placementMenu_BP) {
		placementMenuWidget = CreateWidget<UPlacementMenuWidget>(controller, placementMenu_BP);
		placementMenuWidget->AddToViewport();
	}

	
}

void ALowPolySurvivalCharacter::AddItemStackToInventory(FItemStack &itemstack){

	inventoryComp->AddStack(itemstack);

}

void ALowPolySurvivalCharacter::OpenInventory(ALogistic* logistic){
	//UE_LOG(LogTemp, Warning, TEXT("Open Inventory"));
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

	CrosshairLineTrace(cCrosshairTraceResult, cCrosshairTraceDirection);

	if (cCrosshairTraceResult.GetActor()) {
		ABuildings* targetBuilding = Cast<ABuildings>(cCrosshairTraceResult.GetActor());
		if (targetBuilding) {
			playerHUDWidget->UpdateTargetIndicator(targetBuilding->info);
			playerHUDWidget->ShowTargetIndicator();
		}
		else {
			playerHUDWidget->HideTargetIndicator();
		}
	}

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

	FHitResult hitResult;
	FVector direction;
	CrosshairLineTrace(hitResult, direction);

	FVector dir;
	float length;
	(hitResult.ImpactPoint - GetActorLocation()).ToDirectionAndLength(dir, length);

	//UE_LOG(LogTemp, Warning, TEXT("%f"), length);
	 

	if (length <= attributeComp->GetAttributes().hitRange) {


		ABuildings* building = Cast<ABuildings>(hitResult.GetActor());

		if (building) {
			UE_LOG(LogTemp, Warning, TEXT("%s"), *hitResult.GetActor()->GetName());

			int32 damage = 10;
			
			if (rightHandStack && rightHandStack->IsValid()) {
				damage += rightHandStack->itemInfo->damage;
			}

			building->ApplyDamage(damage, this);
		}


		bIsInHit = true;

		ApplyDamage(1);

	}

	
}

bool ALowPolySurvivalCharacter::CrosshairLineTrace(FHitResult &OUT_hitresult, FVector &OUT_Direction){

	FVector worldLocation;
	controller->DeprojectScreenPositionToWorld(viewX * 0.5f, viewY * 0.5f, worldLocation, OUT_Direction);

	FVector startLocation = worldLocation;
	FVector endLocation = OUT_Direction * 1000 + startLocation;

	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(OUT_hitresult, startLocation, endLocation, ECollisionChannel::ECC_GameTraceChannel2, params);

	if (OUT_hitresult.GetActor()) {
		//DrawDebugLine(GetWorld(), startLocation, hitResult.ImpactPoint, FColor::Red, false, -1.0f, 0, 1.0f);
	}

	//DrawDebugSphere(GetWorld(), OUT_hitresult.ImpactPoint, 50, 10, FColor::Red);


	return true;
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
	bIsHoldingShift = true;
}

void ALowPolySurvivalCharacter::OnShiftReleased(){
	bIsHoldingShift = false;
}

void ALowPolySurvivalCharacter::ToggleInventory(){
	//inventory->AddToPlayerViewport(controller);
	//UE_LOG(LogTemp, Warning, TEXT("Toggle Inventory"));

	inventoryManager->ShowInventory(inventoryComp);
}

void ALowPolySurvivalCharacter::OnInteraction(){
	FHitResult hitResult;
	FVector direction;
	CrosshairLineTrace(hitResult, direction);

	ABuildings* building = Cast<ABuildings>(hitResult.GetActor());

	if (building) {
		currentInteractionBuilding = building;
		building->Interact(this);
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

	if (rightHandStack && rightHandStack->IsValid() && rightHandStack->itemInfo->buildingTemplate_BP) {
		if (rightHandStack->itemInfo->bCanPlace) {
			placementComp->ActivatePlacement(rightHandStack->itemInfo);
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
	
	if (rightHandStack && rightHandStack->IsValid() && rightHandStack->itemInfo->buildingTemplate_BP) {

		ABuildings* itemBuilding = rightHandStack->itemInfo->buildingTemplate_BP->GetDefaultObject<ABuildings>();

		if (rightHandStack->itemInfo->bCanHold) {
			armActor->SetInHandMesh(itemBuilding->GetStaticMesh());
		}
		else {
			armActor->RemoveInHandMesh();
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


