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
class UDataTable;
class USkeletalMesh;



USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FBuildingInfo {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName name;

	UPROPERTY(EditAnywhere)
	int32 durability;

	UPROPERTY(EditAnywhere)
	FDataTableRowHandle itemDrops;

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

	
	

protected:

	bool bIsSkeletalMesh = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Datatable")
	UDataTable* itemDataTable;


	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scene")
	USceneComponent* sceneComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scene")
	USceneComponent* subSceneComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* meshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	USkeletalMeshComponent* skeletalMeshComp;

	FItemDrops* dropInfo;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	void DropItems(ALowPolySurvivalCharacter * player = nullptr);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	virtual void ApplyDamage(int32 amount, ALowPolySurvivalCharacter* causer);
	virtual void Interact(ALowPolySurvivalCharacter* interactor);

	void SetCollisionEnabled(ECollisionEnabled::Type collisionType, bool ignoreCrosshairTrace);
	
	void SetHolo(bool isHolo = true);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void OnInteractBegin();

	UFUNCTION(BlueprintImplementableEvent)
	void OnInteractEnd();

	bool IsSkeletalMesh() const;

	UStaticMeshComponent* GetStaticMeshComp() const;
	UStaticMesh* GetStaticMesh() const;
	USkeletalMesh* GetSkeletalMesh() const;
};
