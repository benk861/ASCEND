// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "AbilityBase.generated.h"

class APlayerCharacter;
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

	UFUNCTION(BlueprintNativeEvent)
	bool CanActivateAbility();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE APlayerCharacter* GetAscendPlayer(){return Player;}

	bool CheckContainment(TArray<AAbilityBase*>& CorrespondingArray);

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	void Reactivate();
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability description")
	FName AbilityName;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability description")
	int32 MaxCharges = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Ability description")
	EAbilityHand Hand;
	
	bool bIsPicked = false;

	bool bActive = true;

	UPROPERTY(BlueprintReadOnly)
	int32 ChargesLeft = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPaperSpriteComponent* Sprite;
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetChargesLeft(int32 NewChargesLeft);
	UFUNCTION(BlueprintCallable)
	void SetMaxCharges(int32 Charges);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetMaxCharges();

	UPROPERTY(EditAnywhere)
	USphereComponent* PickupCollision = nullptr;

private:
	FTransform BegininigTransform;
	FAbilityPicked AbilityPickedDelegate;
	APlayerCharacter* Player;
	bool bShouldBePicked = true;
};
