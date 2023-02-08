// Fill out your copyright notice in the Description page of Project Settings.


#include "ASCEND/Components/AbilityManager.h"
#include "ASCEND/Actors/Characters/PlayerCharacter.h"

UAbilityManager::UAbilityManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAbilityManager::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<APlayerCharacter>(GetOwner());
}

void UAbilityManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!LeftHandAbilitiesArray.IsEmpty())
	{
		for(AAbilityBase* Ability : LeftHandAbilitiesArray)
		{
			FString Name = Ability->AbilityName.ToString();
			int32 Index = LeftHandAbilitiesArray.Find(Ability);
			bool IsActive = Ability->bIsPicked;
			FString Active;
			if(IsActive)
			{
				Active = "Active";
			} else
			{
				Active = "Not active";
			}
			// GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Yellow, String);
			UE_LOG(LogTemp, Warning, TEXT("%s %s %s"), *Name, *FString::FromInt(Index), *Active);
		}
	}
}

void UAbilityManager::ActivatePickedLeftHandAbility()
{
	if(LeftHandAbilitiesArray.IsEmpty())
	{
		return;
	}
	
	for(AAbilityBase* Ability : LeftHandAbilitiesArray)
	{
		if(Ability->bIsPicked && Ability->CanActivateAbility())
		{
			ActivateAbility(Ability);
		}
	}
}

void UAbilityManager::ActivatePickedRightHandAbility()
{
	if(RightHandAbilitiesArray.IsEmpty())
	{
		return;
	}
	
	for(AAbilityBase* Ability : RightHandAbilitiesArray)
	{
		if(Ability->bIsPicked && Ability->CanActivateAbility())
		{
			ActivateAbility(Ability);
		}
	}
}

void UAbilityManager::ActivateAbility(AAbilityBase* Ability)
{
	Ability->UseAbility();
	if(Ability->ChargesLeft == 0)
	{
		GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red, "No Charges. Ability Removed");
		PickAbility(GetNextAbilityInArray(Ability));
		if(Ability->Hand == EAbilityHand::Left)
		{
			LeftHandAbilitiesArray.Remove(Ability);
			Ability->Destroy();
		}
		if(Ability->Hand == EAbilityHand::Right)
		{
			RightHandAbilitiesArray.Remove(Ability);
			Ability->Destroy();
		}
	}
}

void UAbilityManager::PickAbility(AAbilityBase* Ability)
{
	if(!Ability)
	{
		return;
	}
	if(Ability->Hand == EAbilityHand::Left)
	{
		for (AAbilityBase* AbilityInArray : LeftHandAbilitiesArray)
		{
			AbilityInArray->bIsPicked = false;
		}
		Ability->bIsPicked = true;
		
		TArray<AAbilityBase*> RestructuredArray;
		RestructuredArray.Add(Ability);
		
		for(AAbilityBase* AbilityInArray : LeftHandAbilitiesArray)
		{	
			if(AbilityInArray != Ability)
			{
				RestructuredArray.Add(AbilityInArray);
			}
		}
		
		LeftHandAbilitiesArray = RestructuredArray;
	}
	
	if(Ability->Hand == EAbilityHand::Right)
	{
		for (AAbilityBase* AbilityInArray : RightHandAbilitiesArray)
		{
			AbilityInArray->bIsPicked = false;
		}
		Ability->bIsPicked = true;

		TArray<AAbilityBase*> RestructuredArray;
		RestructuredArray.Add(Ability);
		
		for(AAbilityBase* AbilityInArray : RightHandAbilitiesArray)
		{	
			if(AbilityInArray != Ability)
			{
				RestructuredArray.Add(AbilityInArray);
			}
		}
		
		RightHandAbilitiesArray = RestructuredArray;
	}
	GEngine->AddOnScreenDebugMessage(0, 1.f, FColor::Cyan, Ability->AbilityName.ToString() + " picked up");
}

void UAbilityManager::CycleThroughLeftHand()
{
	int32 NumberOfItemsInArray = LeftHandAbilitiesArray.Num();
	if(NumberOfItemsInArray <= 1)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.f, FColor::Red, "You have no other abilities");
		return;
	}
	for (AAbilityBase* AbilityInArray : LeftHandAbilitiesArray)
	{
		if(AbilityInArray->bIsPicked)
		{
			PickAbility(GetNextAbilityInArray(AbilityInArray));
		}
	}
}

void UAbilityManager::CycleThroughRightHand()
{
	int32 NumberOfItemsInArray = RightHandAbilitiesArray.Num();
	if(NumberOfItemsInArray <= 1)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.f, FColor::Red, "You have no other abilities");
		return;
	}
	for (AAbilityBase* AbilityInArray : RightHandAbilitiesArray)
	{
		if(AbilityInArray->bIsPicked)
		{
			PickAbility(GetNextAbilityInArray(AbilityInArray));
		}
	}
}

AAbilityBase* UAbilityManager::GetNextAbilityInArray(AAbilityBase* CurrentAbility)
{
	if(CurrentAbility->Hand == EAbilityHand::Left)
	{
		int32 NumberOfItemsInArray = LeftHandAbilitiesArray.Num() - 1;
		int32 CurrentAbilityArrayIndex = LeftHandAbilitiesArray.Find(CurrentAbility);
		if(CurrentAbilityArrayIndex == NumberOfItemsInArray)
		{
			return LeftHandAbilitiesArray[0];
		}
		if(CurrentAbilityArrayIndex < NumberOfItemsInArray)
		{
			return LeftHandAbilitiesArray[CurrentAbilityArrayIndex + 1];
		}
	}
	if(CurrentAbility->Hand == EAbilityHand::Right)
	{
		int32 NumberOfItemsInArray = RightHandAbilitiesArray.Num() - 1;
		int32 CurrentAbilityArrayIndex = RightHandAbilitiesArray.Find(CurrentAbility);
		if(CurrentAbilityArrayIndex == NumberOfItemsInArray)
		{
			return RightHandAbilitiesArray[0];
		}
		if(CurrentAbilityArrayIndex < NumberOfItemsInArray)
		{
			return RightHandAbilitiesArray[CurrentAbilityArrayIndex + 1];
		}
	}
	return nullptr;
}
