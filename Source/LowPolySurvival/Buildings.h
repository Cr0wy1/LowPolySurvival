// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Buildings.generated.h"



class UStaticMeshComponent;
class ABuildings;
class UItem;
class ALowPolySurvivalCharacter;
struct FItemDrops;
struct FItemInfo;
class UDataTable;



USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FBuildingInfo {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName itemId = "0";

	UPROPERTY(EditAnywhere)
	int32 currentDurability;


	FItemInfo* itemInfo = nullptr;

};



UCLASS()
class LOWPOLYSURVIVAL_API ABuildings : public AActor
{
	GENERATED_BODY()
	
public:	

	

	// Sets default values for this actor's properties
	ABuildings();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	FBuildingInfo info;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Origins")
	TArray<FTransform> origins;
	

protected:

	bool bIsPlaced = false;
	bool bHasPlaceInterface = false;
	bool bIsOverlappingBuilding = false;

	int32 addedMargin = 0;

	UDataTable* itemDataTable;

	UMaterialInterface* meshMaterial = nullptr;


	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scene")
	USceneComponent* sceneComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scene")
	USceneComponent* meshSceneComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* meshComp;

	FItemDrops* dropInfo;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	void DropItems(ALowPolySurvivalCharacter * player = nullptr);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ConstructFromItem(FItemInfo* itemInfo);
	virtual void ApplyDamage(int32 amount, ALowPolySurvivalCharacter* causer);
	virtual void Interact(ALowPolySurvivalCharacter* interactor);

	void SetCollisionEnabled(ECollisionEnabled::Type collisionType, bool ignoreCrosshairTrace);
	
	void SetHolo(bool isHolo = true);
	void SetMaterial(UMaterialInterface* newMaterial);

	void OnPlace();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlaceEvent();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void OnInteractBegin();

	UFUNCTION(BlueprintImplementableEvent)
	void OnInteractEnd();

	void SetMargin(int32 margin);

	void SetSceneTransform(const FTransform &transform);
	void SetMeshSceneTransform(const FTransform &transform);
	void SetMeshTransform(const FTransform &transform);
	
	void SetStaticMesh(UStaticMesh *newMesh);
	
	bool IsOverlappingBuilding() const;

	UStaticMeshComponent* GetStaticMeshComp() const;
	UStaticMesh* GetStaticMesh() const;

	FTransform GetSceneTransform() const;
	FTransform GetMeshSceneTransform() const;
	FTransform GetMeshTransform() const;
	

	bool HasPlaceInterface() const;
};
