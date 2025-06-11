// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

float UAuraDamageGameplayAbility::GetCost(const float InLevel) const
{
	float Cost = 0.f;
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for (FGameplayModifierInfo Mod : CostEffect->Modifiers)
		{
			if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cost);
				break;
			}
		}
	}
	return Cost;
}

float UAuraDamageGameplayAbility::GetCooldown(const float InLevel) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}
	return Cooldown;
}

float UAuraDamageGameplayAbility::GetDamage(const float InLevel) const
{
	return DamageTypes[DamageTag].GetValueAtLevel(InLevel);
}

FText UAuraDamageGameplayAbility::GetAbilityName() const
{
	return AbilityName;
}

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	// EffectSpec作成.
	FGameplayEffectSpecHandle DamageEffectSpec = MakeOutgoingGameplayEffectSpec(DamageEffect, 1.f);

	// Damageを設定.
	for (const TTuple<FGameplayTag, FScalableFloat>& DamageType : DamageTypes)
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpec, DamageType.Key, DamageType.Value.GetValueAtLevel(GetAbilityLevel()));
	}

	// TargetActorに適用.
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageEffectSpec.Data.Get(),UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontage) const
{
	if (TaggedMontage.Num() > 0)
	{
		const int32 Selection = FMath::RandRange(0, TaggedMontage.Num() - 1);
		return TaggedMontage[Selection];
	}
	return FTaggedMontage();
}

