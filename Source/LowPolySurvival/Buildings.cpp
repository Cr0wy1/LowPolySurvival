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
#include "MyGameInstance.h"

// Sets default values
ABuildings::ABuildings() : bHasPlaceInterface(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	sceneComp = CreateAbstractDefaultSubobject<USceneComponent>("Scene");
	SetRootComponent(sceneComp);

	subSceneComp = CreateAbstractDefaultSubobject<USceneComponent>("SubScene");
	subSceneComp->SetupAttachment(sceneComp);

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	meshComp->SetupAttachment(subSceneComp);

	skeletalMeshComp = CreateAbstractDefaultSubobject<USkeletalMeshComponent>("Skeletal Mesh");
	skeletalMeshComp->SetupAttachment(subSceneComp);

	meshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
	
	meshComp->OnComponentBeginOverlap.AddDynamic(this, &ABuildings::OnBeginOverlap);
	meshComp->OnComponentEndOverlap.AddDynamic(this, &ABuildings::OnEndOverlap);
	
	
	
	
}

// Called when the game starts or when spawned
void ABuildings::BeginPlay(){
	Super::BeginPlay();

	

	UMyGameInstance *gameInstance = Cast<UMyGameInstance>(GetGameInstance());
	itemDataTable = gameInstance->GetItemTable();
	
	info.itemInfo = itemDataTable->FindRow<FItemInfo>(info.itemId, FString(""));
	ConstructFromItem(info.itemInfo);

}

void ABuildings::DropItems(ALowPolySurvivalCharacter * player){

	//Drop into Player Inventorys
	if (player) {

		if (itemDataTable && info.itemInfo) {

			if (info.itemInfo->dropType == EDropType::SELF) {
				FItemStack itemStack;
				itemStack.Set(info.itemInfo);

				player->AddItemStackToInventory(itemStack);
			}
			else if (info.itemInfo->dropType == EDropType::FROMTABLE && dropInfo) {
				for (size_t i = 0; i < dropInfo->itemId.Num(); ++i) {

					int32 rand = FMath::Rand() % (dropInfo->itemId[i].amount + 1);

					if (rand != 0) {
						FItemStack itemStack;
						itemStack.itemInfo = itemDataTable->FindRow<FItemInfo>(dropInfo->itemId[i].itemId, FString(""));
						itemStack.amount = rand;


						player->AddItemStackToInventory(itemStack);
					}
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



void ABuildings::ConstructFromItem(FItemInfo * itemInfo){
	info.itemInfo = itemInfo;
	info.currentDurability = info.itemInfo->durability;

	if (info.itemInfo->mesh) {
		SetStaticMesh(info.itemInfo->mesh);
	}


	if (info.itemInfo->dropType == EDropType::FROMTABLE) {
		UMyGameInstance *gameInstance = Cast<UMyGameInstance>(GetGameInstance());
		dropInfo = gameInstance->GetDropsTable()->FindRow<FItemDrops>(FName(*FString::FromInt(info.itemInfo->itemid)), FString(""));
	}
}

void ABuildings::ApplyDamage(int32 amount, ALowPolySurvivalCharacter* causer){

	info.currentDurability -= amount;

	DropItems(causer);

	if (info.currentDurability <= 0) {

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
		meshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	}
	else {
		meshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		if (meshMaterial) {
			meshComp->SetMaterial(0, meshMaterial);
		}
		
	}
}

void ABuildings::SetMaterial(UMaterialInterface * newMaterial){
	if (!meshMaterial) {
		meshMaterial = meshComp->GetMaterial(0);
	}

	meshComp->SetMaterial(0, newMaterial);
}



void ABuildings::OnPlace(){

	bIsPlaced = true;
	ConstructFromItem(info.itemInfo);
}

void ABuildings::OnBeginOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult){
	
	if (!bIsPlaced) {
		UE_LOG(LogTemp, Warning, TEXT("%s : Overlapping"), *GetName());
		if (Cast<ABuildings>(OtherActor)) {
			bIsOverlappingBuilding = true;
		}
	}

}

void ABuildings::OnEndOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex){
	if (!bIsPlaced) {
		bIsOverlappingBuilding = false;
	}
	
}

bool ABuildings::IsSkeletalMesh() const{
	
	if (skeletalMeshComp->SkeletalMesh) {
		//UE_LOG(LogTemp, Warning, TEXT("Is Skeletal Mesh"));

		return true;
	}

	return false;
}

bool ABuildings::IsOverlappingBuilding() const
{
	return bIsOverlappingBuilding;
}

void ABuildings::SetStaticMesh(UStaticMesh * newMesh){
	meshComp->SetStaticMesh(newMesh);
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

bool ABuildings::HasPlaceInterface() const{

	return bHasPlaceInterface;
}

