// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "Item.h"
#include "LowPolySurvivalCharacter.h"

// Sets default values
ABuildings::ABuildings()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(mesh);

}

// Called when the game starts or when spawned
void ABuildings::BeginPlay(){
	Super::BeginPlay();

	dropInfo = info.itemDrops.DataTable->FindRow<FItemDrops>(info.itemDrops.RowName, FString(""));
	
}

void ABuildings::DropItems(ALowPolySurvivalCharacter * player){

	//Drop into Player Inventorys
	if (player) {

		if (itemDataTable) {
			for (size_t i = 0; i < dropInfo->itemId.Num(); ++i) {

				int32 rand = FMath::Rand() % dropInfo->itemId[i].amount;

				if (rand != 0) {
					FItemStack itemStack;
					itemStack.itemInfo = itemDataTable->FindRow<FItemInfo>(dropInfo->itemId[i].itemId, FString(""));
					itemStack.amount = rand;
					player->AddItemStackToInventory(itemStack);
				}
			}
		}
	}
	//Drop into the world
	else {
		for (size_t i = 0; i < dropInfo->itemId.Num(); ++i) {
			int32 rand = FMath::Rand() % dropInfo->itemId[i].amount;

			UE_LOG(LogTemp, Warning, TEXT("Drop %i of %s"), rand, *dropInfo->itemId[i].itemId.ToString());

		}
	}

}

// Called every frame
void ABuildings::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABuildings::ApplyDamage(int32 amount, ALowPolySurvivalCharacter* causer){

	info.durability -= amount;

	DropItems(causer);

	if (info.durability <= 0) {

		Destroy();
	}

}
