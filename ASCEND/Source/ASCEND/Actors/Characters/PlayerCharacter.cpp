// Fill out your copyright notice in the Description page of Project Settings.
#include "ASCEND/Actors/Characters/PlayerCharacter.h"
#include "ASCEND/Components/AbilityManager.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AbilityManager = CreateDefaultSubobject<UAbilityManager>(TEXT("Ability Manger"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

