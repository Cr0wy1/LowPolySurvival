// Fill out your copyright notice in the Description page of Project Settings.


#include "Island.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

// Sets default values
AIsland::AIsland()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Island Mesh");
	SetRootComponent(mesh);
}

// Called when the game starts or when spawned
void AIsland::BeginPlay()
{
	Super::BeginPlay();
	
}



// Called every frame
void AIsland::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIsland::GenerateTerrain(){
	TopDownTrace();
}

void AIsland::TopDownTrace(){
	FVector min, max;
	mesh->GetLocalBounds(min, max);

	UE_LOG(LogTemp, Warning, TEXT("TopDownTrace: %s, %s"), *min.ToString(), *min.ToString());

}

