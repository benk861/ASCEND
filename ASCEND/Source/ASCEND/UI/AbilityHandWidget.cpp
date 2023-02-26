// Fill out your copyright notice in the Description page of Project Settings.


#include "ASCEND/UI/AbilityHandWidget.h"
#include "ASCEND/Actors/Abilities/AbilityBase.h"
#include "ASCEND/Actors/Characters/PlayerCharacter.h"

void UAbilityHandWidget::NativeConstruct()
{
	//TODO remove this unnecessary check
	Super::NativeConstruct();
	if(Hand == EAbilityHand::Left)
	{
		Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetAbilityManager()->LeftHandAbilityDelegate.AddDynamic(this, &UAbilityHandWidget::OnAbilityPicked);
		Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetAbilityManager()->LeftAbilityRemovedDelegate.AddDynamic(this, &UAbilityHandWidget::OnAbilityRemoved);
		Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetAbilityManager()->LeftAbilityUsed.AddDynamic(this, &UAbilityHandWidget::OnAbilityUsed);
	} else if(Hand == EAbilityHand::Right)
	{
		Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetAbilityManager()->RightHandAbilityDelegate.AddDynamic(this, &UAbilityHandWidget::OnAbilityRemoved);
		Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetAbilityManager()->RightAbilityRemovedDelegate.AddDynamic(this, &UAbilityHandWidget::OnAbilityRemoved);
		Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetAbilityManager()->RightAbilityUsed.AddDynamic(this, &UAbilityHandWidget::OnAbilityUsed);
	}
}

void UAbilityHandWidget::OnAbilityUsed_Implementation(AAbilityBase* Ability)
{
	
}

void UAbilityHandWidget::OnAbilityRemoved_Implementation(AAbilityBase* Ability)
{
	
}

void UAbilityHandWidget::OnAbilityPicked_Implementation(AAbilityBase* Ability)
{
	
}
