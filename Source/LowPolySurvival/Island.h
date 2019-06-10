// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Island.generated.h"



class UStaticMeshComponent;



USTRUCT(BlueprintType)
struct LOWPOLYSURVIVAL_API FIslandInfo : public FTableRowBase {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 id = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AIsland> island_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float spawnChance = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 min = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 max = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float minCenterDistance = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float maxCenterDistance = -1;
};



UCLASS()
class LOWPOLYSURVIVAL_API AIsland : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIsland();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Island")
	UStaticMeshComponent* mesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
