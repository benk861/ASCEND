// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Actors/Abilities/AbilityBase.h"
#include "ASCENDGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ASCEND_API UASCENDGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	TArray<AAbilityBase*> Abilities;

	UFUNCTION(BlueprintCallable)
	void ReactivateAbilities();
};
