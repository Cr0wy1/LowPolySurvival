// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LowPolySurvivalCharacter.generated.h"

class ABuildings;
class AConstruction;
class ALogistic;
class AMechArmActor;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputComponent;
class UPlayerHUDWidget;
class UInventoryManagerWidget;
class UInventoryComponent;
class UAttributeComponent;
class UPlacementComponent;
class UWidgetInteractionComponent;
struct FItemStack;



UCLASS(config=Game)
class ALowPolySurvivalCharacter : public ACharacter
{
	GENERATED_BODY()



public:
	ALowPolySurvivalCharacter();

	virtual void Tick(float DeltaTime) override;

protected:

	bool bIsInHit = false;

	APlayerController * controller = nullptr;

	ABuildings* currentInteractionBuilding = nullptr;

	//Viewport Size
	int32 viewX, viewY;

	//bool for Primary key handling
	bool bIsHoldingPrimary = false;
	bool bIsHoldingAlt = false;
	bool bIsHoldingShift = false;
	bool bIsInventoryOpen = false;


	FItemStack* rightHandStack = nullptr;


	//Arm Actor
	AMechArmActor* armActor = nullptr;

	//Widgets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UPlayerHUDWidget> playerHUDWidget_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UInventoryManagerWidget> inventoryManager_BP;

	//COMPONENTS
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Arm Actor")
	UChildActorComponent* armActorComp;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scene")
	USceneComponent* sceneRightHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* meshRightHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	USkeletalMeshComponent* skeletalMeshRightHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interaction")
	UWidgetInteractionComponent* widgetInteractionComp;

	virtual void BeginPlay();

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	UFUNCTION(BlueprintCallable)
	void OnHit();


	void OnPrimaryPressed();
	void OnPrimaryReleased();

	void OnAltPressed();
	void OnAltReleased();

	void OnShiftPressed();
	void OnShiftReleased();

	void OnRPressed();
	void OnIPressed();

	void ToggleInventory();

	void OnInteraction();

	void OnScrollDown();
	void OnScrollUp();
	void OnScroll();

	void UpdateMeshRightHand();


	//MOVEMENT
	/** Handles moving */
	void MoveForward(float Val);
	void MoveRight(float Val);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

public:

	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	UInventoryComponent* inventoryComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	UInventoryComponent* quickInventoryComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	UInventoryComponent* equipmentInventoryComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	UAttributeComponent* attributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Placement")
	UPlacementComponent* placementComp;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	UChildActorComponent* placeWidgetComp;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPlayerHUDWidget* playerHUDWidget = nullptr;
	UInventoryManagerWidget* inventoryManager = nullptr;

	bool CrosshairLineTrace(FHitResult &OUT_hitresult, FVector &OUT_Direction);

	void AddItemStackToInventory(FItemStack &itemstack);

	void OpenInventory(ALogistic* logistic);

	void ApplyDamage(int32 amount, AActor * causer = nullptr);

	APlayerController* GetPlayerController() const;

	void OnInventoryOpen();
	void OnInventoryClose();

	void OnUpdateHandStack();

	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintCallable)
	bool GetIsInHitAnimation()const;

	UFUNCTION(BlueprintCallable)
	void SetIsInHitAnimationb(bool b);

	UFUNCTION(BlueprintCallable)
	AMechArmActor* GetArmActor() const;
};

