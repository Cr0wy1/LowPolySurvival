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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Datatable")
	UDataTable* itemDataTable;


	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* mesh;

	FItemDrops* dropInfo;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	void DropItems(ALowPolySurvivalCharacter * player = nullptr);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	void ApplyDamage(int32 amount, ALowPolySurvivalCharacter* causer);
};