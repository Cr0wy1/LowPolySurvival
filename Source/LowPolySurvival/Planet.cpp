// Fill out your copyright notice in the Description page of Project Settings.


#include "Planet.h"
#include "SimplexNoise.h"
#include "MyGameInstance.h"
#include "TestingLibrary.h"

// Sets default values
APlanet::APlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();

	int32 blockSizeX = radius * 2 * FWorldParams::chunkSize;

	USimplexNoise::NoiseOctaves(noiseMap, 0, 0, blockSizeX, blockSizeX, 3);
	
	UE_LOG(LogTemp, Warning, TEXT("noiseMap: %s"), *UTestingLibrary::GetArrayDebugData(noiseMap));

}

// Called every frame
void APlanet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

