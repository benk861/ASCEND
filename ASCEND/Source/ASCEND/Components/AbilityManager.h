// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASCEND/Actors/Abilities/AbilityBase.h"
#include "Components/ActorComponent.h"
#include "ASCEND/ASCENDGameInstance.h"
#include "AbilityManager.generated.h"

//TODO make it so it's doesn't need separate delegated for each hand
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLeftAbilityPicked, AAbilityBase*, Ability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRightAbilityPicked, AAbilityBase*, Ability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLeftAbilityRemoved, AAbilityBase*, Ability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRightAbilityRemoved, AAbilityBase*, Ability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLeftAbilityUsed, AAbilityBase*, Ability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRightAbilityUsed, AAbilityBase*, Ability);

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
	
	void ActivateAbility(AAbilityBase* Ability);

	void PickAbility(AAbilityBase* Ability);

	AAbilityBase* GetNextAbilityInArray(AAbilityBase* CurrentAbility);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<AAbilityBase*> LeftHandAbilitiesArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<AAbilityBase*> RightHandAbilitiesArray;

	FLeftAbilityPicked LeftHandAbilityDelegate;
	FRightAbilityPicked RightHandAbilityDelegate;
	FLeftAbilityRemoved LeftAbilityRemovedDelegate;
	FRightAbilityRemoved RightAbilityRemovedDelegate;
	FLeftAbilityUsed LeftAbilityUsed;
	FRightAbilityUsed RightAbilityUsed;

	UFUNCTION(BlueprintCallable)
	int32 GetAbilityIndexInArray(AAbilityBase* Ability);
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	APlayerCharacter* Character = nullptr;

	TArray<AAbilityBase*>& FindCorrespondingArray(AAbilityBase* Ability);
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
