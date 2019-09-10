// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TestingLibrary.generated.h"



USTRUCT()
struct LOWPOLYSURVIVAL_API FArrayDebugData {
	GENERATED_BODY()

	size_t num = 0;
	size_t allocatedSize = 0;
	size_t slackAmount = 0;
	size_t typeSize = 0;
	size_t max = 0;


	FArrayDebugData(){}

	FArrayDebugData(const TArray<float> &arr) {
		num = arr.Num();
		allocatedSize = arr.GetAllocatedSize();
		slackAmount = arr.GetSlack();
		typeSize = arr.GetTypeSize();
		max = arr.Max();
	}

	FArrayDebugData& operator+=(const FArrayDebugData &other) {
		num += other.num;
		allocatedSize += other.allocatedSize;
		slackAmount += other.slackAmount;
		typeSize += other.typeSize;
		max += other.max;

		return *this;
	}

	FString ToString() const {

		FString returnString = "TArray<float> debugInfo{" +
			FString("Num:") + FString::FromInt(num) +
			", AllocatedSize:" + FString::FromInt(allocatedSize) +
			", SlackAmount:" + FString::FromInt(slackAmount) +
			", TypeSize:" + FString::FromInt(typeSize) +
			", Max:" + FString::FromInt(max);

		return returnString;
	}
};
/**
 * 
 */
UCLASS()
class LOWPOLYSURVIVAL_API UTestingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

public:

	UFUNCTION(BlueprintCallable, Category = "Testing")
		static void TArrayTest();

	UFUNCTION(BlueprintCallable, Category = "Testing")
		static FString GetArrayDebugData(const TArray<float> &arr);

	static FString GetArrayDebugData(const TArray<TArray<float>> &arr);
};
