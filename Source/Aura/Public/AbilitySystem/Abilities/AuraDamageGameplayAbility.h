// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	float GetCost(const float InLevel) const;

	float GetCooldown(const float InLevel) const;

	float GetDamage(const float InLevel) const;

	FText GetAbilityName() const;
	
protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageEffect{};

	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DamageTag;

	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontage) const;
};
