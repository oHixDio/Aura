// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

class UAuraAbilitySystemComponent;

/** GameplayEffect → AbilitySystemComponent → WidgetController デリゲードを通してEffectからGameplauTags(AssetTags)を取得する. */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEffectAssetTags, const FGameplayTagContainer& /* AssetTags */);

DECLARE_MULTICAST_DELEGATE(FAbilitiesGivenSignature);

DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnUpdateAbilityStatuses, const FGameplayTag& /* AbilityTag */, const FGameplayTag& /* StatusTag */, const int32 /* AbilityLevel */);

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnEquipAbility, const FGameplayTag& /* AbilityTag */, const FGameplayTag& /* PrevInputTag */, const FGameplayTag& /* NewInputTag */);

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

	FOnUpdateAbilityStatuses OnUpdateAbilityStatuses;

	FOnEquipAbility OnEquipAbility;

	// AbilitiesGivenDelegateのバインド順序が未確定であるため、付与後かどうかを判断するフラグ。
	bool bStartupAbilitiesGiven = false;

	/** StartupAbilitiesをキャラクターに付与する. */
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);
	
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);

	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	void AbilityInputTagHeld(const FGameplayTag& InputTag);

	void ForEachAbility(const FForEachAbility& Delegate);

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	void UpdateAbilityStatuses(const int32 Level);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag);
	
protected:
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle) const;

	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& PrevInputTag, const FGameplayTag& NewInputTag);

	virtual void OnRep_ActivateAbilities() override;

private:
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatuses(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const int32 AbilityLevel);
};
