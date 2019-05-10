// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "Item.h"
#include "LowPolySurvivalCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/SkeletalMesh.h"

// Sets default values
ABuildings::ABuildings()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	sceneComp = CreateAbstractDefaultSubobject<USceneComponent>("Scene");
	SetRootComponent(sceneComp);

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	meshComp->SetupAttachment(sceneComp);

	skeletalMeshComp = CreateAbstractDefaultSubobject<USkeletalMeshComponent>("Skeletal Mesh");
	skeletalMeshComp->SetupAttachment(sceneComp);

	meshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
	
	meshComp->OnComponentBeginOverlap.AddDynamic(this, &ABuildings::OnBeginOverlap);
	meshComp->OnComponentEndOverlap.AddDynamic(this, &ABuildings::OnEndOverlap);
	
	
}

// Called when the game starts or when spawned
void ABuildings::BeginPlay(){
	Super::BeginPlay();



	dropInfo = info.itemDrops.GetRow<FItemDrops>(FString("")); // info.itemDrops.DataTable->FindRow<FItemDrops>(info.itemDrops.RowName, FString(""));
	
}

void ABuildings::DropItems(ALowPolySurvivalCharacter * player){

	//Drop into Player Inventorys
	if (player) {

		if (itemDataTable) {
			for (size_t i = 0; i < dropInfo->itemId.Num(); ++i) {

				int32 rand = FMath::Rand() % (dropInfo->itemId[i].amount + 1);

				if (rand != 0) {
					FItemStack itemStack;
					itemStack.itemInfo = itemDataTable->FindRow<FItemInfo>(dropInfo->itemId[i].itemId, FString(""));
					itemStack.amount = rand;

					//UE_LOG(LogTemp, Warning, TEXT("size info: %i, meta: %i, name: %i"), sizeof(itemStack.itemInfo), sizeof(itemStack.itemInfo->meta), sizeof(itemStack.itemInfo->name));


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

void ABuildings::Interact(ALowPolySurvivalCharacter * interactor){

	//UE_LOG(LogTemp, Warning, TEXT("Buildings: Interact"));

	OnInteractBegin();
}

void ABuildings::SetCollisionEnabled(ECollisionEnabled::Type collisionType, bool ignoreCrosshairTrace){
	meshComp->SetCollisionEnabled(collisionType);

	if (ignoreCrosshairTrace) {
		
	}
	else {
		meshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
	}
	
	
}

void ABuildings::SetHolo(bool isHolo){
	if (isHolo) {
		meshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		//meshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
		meshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
		//meshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else {
		meshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		//meshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
		//meshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		//meshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void ABuildings::OnBeginOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult){
	UE_LOG(LogTemp, Warning, TEXT("%s : Overlapping"), *GetName());

}

void ABuildings::OnEndOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
}

bool ABuildings::IsSkeletalMesh() const{
	
	if (skeletalMeshComp->SkeletalMesh) {
		//UE_LOG(LogTemp, Warning, TEXT("Is Skeletal Mesh"));

		return true;
	}

	return false;
}

UStaticMeshComponent * ABuildings::GetStaticMeshComp() const{
	return meshComp;
}

UStaticMesh * ABuildings::GetStaticMesh() const{
	return meshComp->GetStaticMesh();
} 

USkeletalMesh * ABuildings::GetSkeletalMesh() const{

	return skeletalMeshComp->SkeletalMesh;
}

