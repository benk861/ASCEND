// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "AbilityBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityPicked, AAbilityBase*, ActiveAbility);

class UPaperSpriteComponent;
UENUM(BlueprintType)
enum class EAbilityHand : uint8
{
	Right,
	Left
};

UCLASS()
class ASCEND_API AAbilityBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AAbilityBase();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent)
	void UseAbility();

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability description")
	FName AbilityName;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability description")
	int32 MaxCharges = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Ability description")
	EAbilityHand Hand;
	
	bool bIsPicked = false;
	
	int32 ChargesLeft = 0;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	USphereComponent* PickupCollision = nullptr;

	UPROPERTY(EditAnywhere)
	UPaperSpriteComponent* Sprite;

private:
	FAbilityPicked AbilityPickedDelegate;
};
