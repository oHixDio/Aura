// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

class UAuraAbilitySystemComponent;

/** GameplayEffect → AbilitySystemComponent → WidgetController デリゲードを通してEffectからGameplauTags(AssetTags)を取得する. */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEffectAssetTags, const FGameplayTagContainer&);

DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitiesGivenSignature, UAuraAbilitySystemComponent*);

DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilityActorInfoSet();

	FOnEffectAssetTags OnEffectAssetTags;

	FAbilitiesGivenSignature AbilitiesGivenDelegate;

	// AbilitiesGivenDelegateのバインド順序が未確定であるため、付与後かどうかを判断するフラグ。
	bool bStartupAbilitiesGiven = false;

	/** StartupAbilitiesをキャラクターに付与する. */
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);

	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	void AbilityInputTagHeld(const FGameplayTag& InputTag);

	void ForEachAbility(const FForEachAbility& Delegate);

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	
protected:
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle) const;

	virtual void OnRep_ActivateAbilities() override;
};
