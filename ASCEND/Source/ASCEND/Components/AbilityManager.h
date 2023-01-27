// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASCEND/Actors/Abilities/AbilityBase.h"
#include "Components/ActorComponent.h"
#include "AbilityManager.generated.h"

class APlayerCharacter;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASCEND_API UAbilityManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAbilityManager();

	UFUNCTION(BlueprintCallable)
	void ActivatePickedLeftHandAbility();
	
	UFUNCTION(BlueprintCallable)
	void ActivatePickedRightHandAbility();

	UFUNCTION(BlueprintCallable)
	void CycleThroughLeftHand();
	
	UFUNCTION(BlueprintCallable)
	void CycleThroughRightHand();

	UFUNCTION(BlueprintNativeEvent)
	bool CanActivateAbility();
	
	void ActivateAbility(AAbilityBase* Ability);

	void PickAbility(AAbilityBase* Ability);

	AAbilityBase* GetNextAbilityInArray(AAbilityBase* CurrentAbility);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<AAbilityBase*> LeftHandAbilitiesArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<AAbilityBase*> RightHandAbilitiesArray;
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	APlayerCharacter* Character;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
