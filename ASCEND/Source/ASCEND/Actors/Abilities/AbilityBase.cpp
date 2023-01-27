// Fill out your copyright notice in the Description page of Project Settings.


#include "ASCEND/Actors/Abilities/AbilityBase.h"

#include "PaperSpriteComponent.h"
#include "ASCEND/Actors/Characters/PlayerCharacter.h"
#include "Components/SphereComponent.h"

AAbilityBase::AAbilityBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Pickup collision"));
	Sprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	Sprite->SetupAttachment(PickupCollision);
}

void AAbilityBase::BeginPlay()
{
	Super::BeginPlay();
	ChargesLeft = MaxCharges;
	PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &AAbilityBase::OnOverlap);
}

void AAbilityBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAbilityBase::UseAbility_Implementation()
{
	GEngine->AddOnScreenDebugMessage(0, 1.f, FColor::Red, "- 1 charge");
	ChargesLeft--;
}

bool AAbilityBase::CanActivateAbility_Implementation()
{
	return true;
}

void AAbilityBase::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	Player = Cast<APlayerCharacter>(OtherActor);
	UAbilityManager* AbilityManager = Player->GetAbilityManager();
	if(!AbilityManager)
	{
		return;
	}
	if(Hand == EAbilityHand::Left)
	{
		if(!AbilityManager->LeftHandAbilitiesArray.Contains(this))
		{
			AbilityManager->LeftHandAbilitiesArray.AddUnique(this);
		}
		else
		{
			ChargesLeft += MaxCharges;
		}
		
	} else if(Hand == EAbilityHand::Right)
	{
		if(!AbilityManager->RightHandAbilitiesArray.Contains(this))
		{
			AbilityManager->RightHandAbilitiesArray.AddUnique(this);
		}
		else
		{
			ChargesLeft += MaxCharges;
		}
	}

	PickupCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Player->GetAbilityManager()->PickAbility(this);
	if(AbilityPickedDelegate.IsBound())
	{
		AbilityPickedDelegate.Broadcast(this);
	}
	Sprite->SetHiddenInGame(true);
	this->AttachToActor(Player, FAttachmentTransformRules::SnapToTargetIncludingScale);
}
