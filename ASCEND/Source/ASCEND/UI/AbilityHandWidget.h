// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityHandWidget.generated.h"

enum class EAbilityHand : uint8;
/**
 * 
 */
UCLASS()
class ASCEND_API UAbilityHandWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnAbilityPicked(AAbilityBase* Ability);

	UFUNCTION(BlueprintNativeEvent)
	void OnAbilityRemoved(AAbilityBase* Ability);

	UFUNCTION(BlueprintNativeEvent)
	void OnAbilityUsed(AAbilityBase* Ability);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAbilityHand Hand;

};
