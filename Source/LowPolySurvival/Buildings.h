// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Buildings.generated.h"



class UStaticMeshComponent;
class ABuildings;


USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FDropInfo {
	GENERATED_BODY()


		UPROPERTY(EditAnywhere)
		TSubclassOf<ABuildings> buildingsBP;

		UPROPERTY(EditAnywhere)
		int32 amount;



};



USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FBuildingInfo {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName name;

	UPROPERTY(EditAnywhere)
	int32 durability;

	UPROPERTY(EditAnywhere)
	TArray<FDropInfo> drops;

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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* mesh;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	void ApplyDamage(int32 amount);
};
