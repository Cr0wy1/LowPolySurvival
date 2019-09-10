// Fill out your copyright notice in the Description page of Project Settings.


#include "TestingLibrary.h"

void UTestingLibrary::TArrayTest() {

	TArray<float> arr;
	UE_LOG(LogTemp, Warning, TEXT("declare: %s"), *GetArrayDebugData(arr) );

	arr.Init(1.0f, 99);
	UE_LOG(LogTemp, Warning, TEXT("Init(1.0f, 99): %s"), *GetArrayDebugData(arr));

	arr.Reset();
	UE_LOG(LogTemp, Warning, TEXT("Reset: %s"), *GetArrayDebugData(arr));

	arr.AddUninitialized(10);
	UE_LOG(LogTemp, Warning, TEXT("AddUninitialized(10): %s"), *GetArrayDebugData(arr));
	UE_LOG(LogTemp, Warning, TEXT("[0]: %f"), arr[0]);

	arr.Empty();
	UE_LOG(LogTemp, Warning, TEXT("Empty: %s"), *GetArrayDebugData(arr));

}

FString UTestingLibrary::GetArrayDebugData(const TArray<float>& arr){

	return FArrayDebugData(arr).ToString();
}

FString UTestingLibrary::GetArrayDebugData(const TArray<TArray<float>> &arr) {
	
	FArrayDebugData debugData;

	for (size_t x = 0; x < arr.Num(); x++){
		debugData += FArrayDebugData(arr[x]);
	}

	return debugData.ToString();
}
