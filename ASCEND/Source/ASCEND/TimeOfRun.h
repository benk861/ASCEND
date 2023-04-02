// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TimeOfRun.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ASCEND_API UTimeOfRun : public UObject
{
	GENERATED_BODY()
public:
	UTimeOfRun() = default;
	UTimeOfRun(FString start, FString target, int length = 10):
		Length(length),
		Start(start),
		Target(target)
		{};
	~UTimeOfRun() = default;
	UFUNCTION(BlueprintCallable)
	int AddTime(int time);
	UFUNCTION(BlueprintCallable)
	FString GetStart() const;
	UFUNCTION(BlueprintCallable)
	FString GetTarget() const;
	UFUNCTION(BlueprintCallable)
	void SetStart(FString start);
	UFUNCTION(BlueprintCallable)
	void SetTarget(FString target);
	UFUNCTION(BlueprintCallable)
	TArray<int> GetTimes() const;
	UPROPERTY(BlueprintReadWrite)
	int Length = 10;

private:
	UPROPERTY()
	FString Start;
	UPROPERTY()
	FString Target;
	UPROPERTY()
	TArray<int> Time;
};
