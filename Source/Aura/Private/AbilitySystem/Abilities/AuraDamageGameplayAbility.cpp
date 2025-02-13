// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

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
