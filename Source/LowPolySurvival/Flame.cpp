// Fill out your copyright notice in the Description page of Project Settings.

#include "Flame.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"

AFlame::AFlame() {

	flameParticleComp = CreateDefaultSubobject<UParticleSystemComponent>("Flame Particle");
	SetRootComponent(flameParticleComp);

	flameLightComp = CreateDefaultSubobject<UPointLightComponent>("Flame Light");
	flameLightComp->SetupAttachment(flameParticleComp);
}

void AFlame::ActivateFlame(){

}

void AFlame::DeactivateFlame(){

}
