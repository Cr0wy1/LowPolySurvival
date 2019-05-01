// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MechArmActor.generated.h"


class UStaticMeshComponent;
class USkeletalMeshComponent;
class UAnimationAsset;


UCLASS()
class LOWPOLYSURVIVAL_API AMechArmActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMechArmActor();

protected:

	bool bIsInHand = false;
	
	//COMPONENTS
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* armMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	USkeletalMeshComponent* handSkeletalComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* inHandMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scene")
	USceneComponent* sceneInHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* meshRightHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	USkeletalMeshComponent* skeletalMeshRightHand;
	

	//PROPERTIES
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* fistHandAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* flatHandAnimation;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetHandRotation(float value);
	
	void SetHandFlat();
	void SetHandFist();

	void SetInHandMesh(UStaticMesh* mesh);
	void RemoveInHandMesh();

	UFUNCTION(BlueprintCallable)
	bool IsInHand() const;
};
