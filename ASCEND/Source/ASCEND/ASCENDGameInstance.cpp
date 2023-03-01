// Fill out your copyright notice in the Description page of Project Settings.


#include "ASCENDGameInstance.h"

void UASCENDGameInstance::ReactivateAbilities()
{
	for (AAbilityBase* Ability : Abilities)
	{
		if(Ability)Ability->Reactivate();
	}
}
