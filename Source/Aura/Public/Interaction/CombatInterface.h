// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class UNiagaraSystem;

namespace CombatSocketName
{
	extern const FName WeaponSocket;
	extern const FName LeftHandSocket;
	extern const FName RightHandSocket;
	extern const FName TailSocket;
}

USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* Montage{};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MontageTag{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag SocketTag{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* ImpactSound{};
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual float GetPlayerLevel() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCombatSocketLocation(const FGameplayTag& SocketTag) const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetFacingTarget(const FVector& TargetLocation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FTaggedMontage> GetAttackMontages() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FTaggedMontage GetAttackMontageByMontageTag(const FGameplayTag& MontageTag) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UNiagaraSystem* GetBloodEffect();

	virtual void Die() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;

};
