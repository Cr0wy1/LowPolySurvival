// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SimplexNoise.h"
#include "GridComponent.generated.h"



struct FBlockInfo;



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOWPOLYSURVIVAL_API UGridComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGridComponent();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FNoiseParams noiseParams;
	// Called when the game starts
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FVector gridSize;

	TArray<TArray<TArray<FBlockInfo>>> grid;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void InitGrid();

	UFUNCTION(BlueprintCallable)
	void RandomizeGrid();

	UFUNCTION(BlueprintCallable)
	void ApplySimplexNoise2D();

	UFUNCTION(BlueprintCallable)
	void ApplySimplexNoise3D();

	UFUNCTION(BlueprintCallable)
	void ApplyDiamondSquare();

	const TArray<TArray<TArray<FBlockInfo>>>* GetGridPointer() const;
};
