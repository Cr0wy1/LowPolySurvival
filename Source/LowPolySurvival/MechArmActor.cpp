// Fill out your copyright notice in the Description page of Project Settings.

#include "MechArmActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimationAsset.h"

// Sets default values
AMechArmActor::AMechArmActor(){

	PrimaryActorTick.bCanEverTick = false;

	armMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("Arm Mesh");
	SetRootComponent(armMeshComp);
	armMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	handSkeletalComp = CreateDefaultSubobject<USkeletalMeshComponent>("Hand Skeletal");
	handSkeletalComp->SetupAttachment(armMeshComp, FName("hand"));
	handSkeletalComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	inHandMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("in Hand Mesh");
	inHandMeshComp->SetupAttachment(handSkeletalComp, FName("tool_socket"));
	inHandMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AMechArmActor::BeginPlay()
{
	Super::BeginPlay();
	
	SetHandFist();
}

// Called every frame
void AMechArmActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMechArmActor::SetHandRotation(float value){
	handSkeletalComp->SetRelativeRotation(FRotator(0.0f, value, 0.0f));
}

void AMechArmActor::SetHandFlat(){
	if (flatHandAnimation) {
		handSkeletalComp->PlayAnimation(flatHandAnimation, true);
	}
	SetHandRotation(-90.0f);
}

void AMechArmActor::SetHandFist(){
	if (fistHandAnimation) {
		handSkeletalComp->PlayAnimation(fistHandAnimation, true);
	}
	SetHandRotation(0.0f);
}

void AMechArmActor::SetInHandMesh(UStaticMesh * mesh){

	SetHandFist();

	if (inHandMeshComp->GetStaticMesh() != mesh) {
		inHandMeshComp->SetStaticMesh(mesh);
	}
	
	inHandMeshComp->SetVisibility(true);
	bIsInHand = true;
}

void AMechArmActor::RemoveInHandMesh(){

	SetHandRotation(45.0f);

	inHandMeshComp->SetVisibility(false);
	bIsInHand = false;
}

bool AMechArmActor::IsInHand() const{
	return bIsInHand;
}

