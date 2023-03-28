// Fill out your copyright notice in the Description page of Project Settings.


#include "ASCEND/Actors/Abilities/AbilityBase.h"
#include "PaperSpriteComponent.h"
#include "ASCEND/Actors/Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	BegininigTransform = this->GetTransform();
	ChargesLeft = MaxCharges;
	PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &AAbilityBase::OnOverlap);
	CameraManager = UGameplayStatics::UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	BeginningRotation = Sprite->GetRelativeRotation();
}

void AAbilityBase::SetChargesLeft(int32 NewChargesLeft)
{
	ChargesLeft = NewChargesLeft;
}

void AAbilityBase::SetMaxCharges(int32 Charges)
{
	MaxCharges = Charges;
}

int32 AAbilityBase::GetMaxCharges()
{
	return MaxCharges;
}

void AAbilityBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bActive && CameraManager)
	{
		RotateSpriteToPlayer();
	}
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

bool AAbilityBase::CheckContainment(TArray<AAbilityBase*>& CorrespondingArray)
{
	for (AAbilityBase* AbilityInArray : CorrespondingArray)
	{
		if (AbilityInArray->AbilityName == AbilityName)
		{
			if (AbilityInArray->ChargesLeft != MaxCharges)
			{
				AbilityInArray->ChargesLeft = MaxCharges;
				bShouldBePicked = true;
			}
			else bShouldBePicked = false;
			return true;
		}
	}
	return false;
}

void AAbilityBase::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		UAbilityManager* AbilityManager = Player->GetAbilityManager();
		if (!AbilityManager)
		{
			return;
		}
		if (Hand == EAbilityHand::Left)
		{
			if (!CheckContainment(AbilityManager->LeftHandAbilitiesArray))
			{
				bPickIt = true;
				AbilityManager->LeftHandAbilitiesArray.AddUnique(this);
				Player->GetAbilityManager()->PickAbility(this);
			}
			else
			{
				bPickIt = false;
				Player->GetAbilityManager()->PickAbility(this);
			}

		}
		else if (Hand == EAbilityHand::Right)
		{
			if (!CheckContainment(AbilityManager->RightHandAbilitiesArray))
			{
				bPickIt = true;
				AbilityManager->LeftHandAbilitiesArray.AddUnique(this);
				Player->GetAbilityManager()->PickAbility(this);
			}
			else
			{
				bPickIt = false;
				Player->GetAbilityManager()->PickAbility(this);
			}
		}

		if (bShouldBePicked)
		{
			PickupCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			if (AbilityPickedDelegate.IsBound())
			{
				AbilityPickedDelegate.Broadcast(this);
			}
			Sprite->SetHiddenInGame(true);
			this->AttachToActor(Player, FAttachmentTransformRules::SnapToTargetIncludingScale);
		}
	}
}

void AAbilityBase::Reactivate()
{
	if (!bActive)
	{
		ChargesLeft = MaxCharges;
		this->SetActorTransform(BegininigTransform);
		PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Sprite->SetHiddenInGame(false);
		bActive = true;
	}
}

void AAbilityBase::RotateSpriteToPlayer()
{
	FRotator Direction = UKismetMathLibrary::FindLookAtRotation(CameraManager->GetCameraLocation(), this->GetActorLocation());
	Direction.Roll = 0;
	Direction.Pitch = 0;
	Sprite->SetWorldRotation(Direction + FRotator(0,90,0) + BeginningRotation);
}
