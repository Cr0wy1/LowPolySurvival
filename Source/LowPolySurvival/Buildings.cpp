// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

// Sets default values
ABuildings::ABuildings()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(mesh);

}

// Called when the game starts or when spawned
void ABuildings::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABuildings::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABuildings::ApplyDamage(int32 amount){

	info.durability -= amount;

	if (info.durability <= 0) {

		for (size_t i = 0; i < info.drops.Num(); ++i){
			for (size_t j = 0; j < info.drops[i].amount; ++j) {
				if (info.drops[i].buildingsBP) {
					GetWorld()->SpawnActor<ABuildings>(info.drops[i].buildingsBP);
				}
			}
		}

		

		Destroy();
	}


}

