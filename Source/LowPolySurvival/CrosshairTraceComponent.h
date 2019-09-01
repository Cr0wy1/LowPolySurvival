// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameStructs.h"
#include "CrosshairTraceComponent.generated.h"



class APlayerController;



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOWPOLYSURVIVAL_API UCrosshairTraceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	FCrosshairResult crosshairResult;

	// Sets default values for this component's properties
	UCrosshairTraceComponent();

protected:

	APlayerController * playerController = nullptr;
	FCollisionQueryParams traceParams; 


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	bool bIgnoreOwner = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float traceLength = 1000.0f;

	// Called when the game starts
	virtual void BeginPlay() override;

	bool CrosshairLineTrace(FHitResult &OUT_hitresult, FVector &OUT_Direction);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FHitResult GetHitResult() const { return crosshairResult.hitResult; }
	FVector GetHitDirection() const { return crosshairResult.hitDirection;  }

	const FCrosshairResult* GetCrosshairResultPtr() const { return &crosshairResult; }
	
};
