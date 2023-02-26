// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySlot.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class ASCEND_API UAbilitySlot : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct() override;
	
	UPROPERTY()
	UTextBlock* ChargesLeft;

	UPROPERTY()
	UImage* AbilitySprite;
};
