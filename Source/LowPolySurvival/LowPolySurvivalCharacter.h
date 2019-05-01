// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LowPolySurvivalCharacter.generated.h"

class ABuildings;
class AConstruction;
class AMechArmActor;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputComponent;
class UDecalComponent;
class UAnimMontage;
class UPlayerHUDWidget;
class UInventoryManagerWidget;
class UItem;
class UInventoryComponent;
class UAttributeComponent;
struct FItemInfo;
struct FItemStack;
enum class EInvType : uint8;



USTRUCT()
struct FPlayerAttributes {

	GENERATED_BODY()

	int32 health = 100;
	int32 stamina = 100;
	int32 food = 100;
	int32 thirstSaturation = 100;
	int32 hungerSaturation = 100;
	float hitRange = 200.0f;

};



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
	bool bIsInventoryOpen = false;

	//cooldown for PrimaryHit
	float primaryHitCooldown = 0.2f;
	float nextHitSeconds = 0;

	FItemStack* rightHandStack = nullptr;

	//Attributes
	FPlayerAttributes attributes;

	//Arm Actor
	AMechArmActor* armActor = nullptr;

	//Widgets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UPlayerHUDWidget> playerHUDWidget_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UInventoryManagerWidget> inventoryManager_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* hitAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* hitMontage;

	//COMPONENTS
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Arm Actor")
	UChildActorComponent* armActorComp;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Scene")
	USceneComponent* sceneRightHand;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* meshRightHand;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USkeletalMeshComponent* skeletalMeshRightHand;


	virtual void BeginPlay();

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	UFUNCTION(BlueprintCallable)
	void OnHit();

	FHitResult CrosshairLineTrace();

	void OnPrimaryPressed();
	void OnPrimaryReleased();

	void ToggleInventory();

	void OnInteraction();

	void OnScrollDown();
	void OnScrollUp();

	void UpdateMeshRightHand();

	int32 GetPlayerHealth() const;

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

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPlayerHUDWidget* playerHUDWidget = nullptr;
	UInventoryManagerWidget* inventoryManager = nullptr;

	void AddItemStackToInventory(FItemStack &itemstack);

	void OpenInventory(AConstruction* construction);

	void ApplyDamage(int32 amount, AActor * causer = nullptr);

	APlayerController* GetPlayerController() const;

	void OnInventoryOpen();
	void OnInventoryClose();


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

