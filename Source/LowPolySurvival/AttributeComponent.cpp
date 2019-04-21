// Fill out your copyright notice in the Description page of Project Settings.

#include "AttributeComponent.h"


// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateStamina();
}

void UAttributeComponent::UpdateStamina() {

	if (GetWorld()->TimeSeconds - lastStaminaReloadSeconds > (attributes.staminaRegDuration / 100)) {
		if (attributes.stamina < attributes.staminaMax) {
			ChargeStamina(1);
			lastStaminaReloadSeconds = GetWorld()->TimeSeconds;
		}
	}
}

void UAttributeComponent::ChargeHealth(int32 amount) {
	if (amount + attributes.health >= attributes.healthMax) {
		attributes.health = attributes.healthMax;
	}
	else {
		attributes.health += amount;
	}
}

void UAttributeComponent::ChargeStamina(int32 amount) {
	if (amount + attributes.stamina >= attributes.staminaMax) {
		attributes.stamina = attributes.staminaMax;
	}
	else {
		attributes.stamina += amount;
	}
}

void UAttributeComponent::RemoveHealth(int32 amount) {
	if (attributes.health - amount <= 0) {
		attributes.health = 0;
	}
	else {
		attributes.health -= amount;
	}
}

void UAttributeComponent::RemoveStamina(int32 amount) {
	if (attributes.stamina - amount <= 0) {
		attributes.stamina = 0;
	}
	else {
		attributes.stamina -= amount;
	}
}


//void UAttributeComponent::ApplyConsumeBuff(FConsumeBuff buff) {
//
//	ChargeHealth(buff.healthCharge);
//	ChargeStamina(buff.StaminaCharge);
//
//
//}

FAttributes UAttributeComponent::GetAttributes() const {
	return attributes;
}

FAttributes & UAttributeComponent::GetAttributesRef(){
	return attributes;
}

