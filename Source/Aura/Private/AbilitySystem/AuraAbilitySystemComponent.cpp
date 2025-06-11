// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemFunctionLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	// StartupGameplayAbilityを生成.
	for (const TSubclassOf<UGameplayAbility> AbilityClass : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			// StartupInputTagをRuntime時に追加する.
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraAbility->StartupInputTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			// ここで付与.
			// 付与時にはActiveにしない.
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec);
			// ClientEquipAbility(GetAbilityTagFromSpec(AbilitySpec), FGameplayTag::EmptyTag,AuraAbility->StartupInputTag);
		}
	}
	
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	// 現在付与されているアビリティを走査.
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	// 現在付与されているアビリティを走査.
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			// アビリティが入力されたことを通知する.
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	// 並列実行安全にする。
	FScopedAbilityListLock ActiveScopeLock(*this);
	
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
	
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (const FGameplayTag& Tag : AbilitySpec.Ability.Get()->GetAssetTags())
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}

	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Input"))))
		{
			return Tag;
		}
	}
	
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return Tag;
		}
	}
	
	return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);

	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		for (FGameplayTag Tag : Spec.Ability.Get()->GetAssetTags())
		{
			if (Tag.MatchesTag(AbilityTag))
			{
				return &Spec;
			}
		}
	}
	return nullptr;
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(const int32 Level)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemFunctionLibrary::GetAbilityInfo(GetAvatarActor());
	check(AbilityInfo);
	for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (Level < Info.LevelRequirement)
		{
			continue;
		}
		if (!Info.AbilityTag.IsValid())
		{
			continue;
		}
		if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			FGameplayAbilitySpec AbilitySpec(Info.Ability, 1);
			const FGameplayTag& StatusTag = FAuraGameplayTags::Get().Abilities_Status_Eligible;
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(StatusTag);
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec);
			ClientUpdateAbilityStatuses(Info.AbilityTag, StatusTag, 1);
		}
	}
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);

	if (FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FGameplayTag& PrevInputTag = GetInputTagFromSpec(*Spec);
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

		// 装備したいアビリティの前の入力タグを全てのアビリティから削除する
		for (FGameplayAbilitySpec& ActivateSpec : GetActivatableAbilities())
		{
			ActivateSpec.GetDynamicSpecSourceTags().RemoveTag(InputTag);
		}

		Spec->GetDynamicSpecSourceTags().RemoveTag(PrevInputTag);

		// 指定アビリティに新しい入力タグを追加
		Spec->GetDynamicSpecSourceTags().AddTag(InputTag);

		// 指定アビリティを UnLocked → Equipped にステータス変更
		if (Spec->GetDynamicSpecSourceTags().HasTagExact(GameplayTags.Abilities_Status_UnLocked))
		{
			Spec->GetDynamicSpecSourceTags().RemoveTag(GameplayTags.Abilities_Status_UnLocked);
			Spec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Equipped);
		}
		
		MarkAbilitySpecDirty(*Spec);
		ClientEquipAbility(AbilityTag, PrevInputTag, InputTag);
		ClientUpdateAbilityStatuses(AbilityTag, GameplayTags.Abilities_Status_Equipped, Spec->Level);
	}
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		bool bSpendPoint = false;
		FGameplayTag Status = GetStatusTagFromSpec(*Spec);
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

		// SpellPointを消費してSpellをアンロックする
		if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
		{
			Spec->GetDynamicSpecSourceTags().RemoveTag(GameplayTags.Abilities_Status_Eligible);
			Spec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_UnLocked);
			Status = GameplayTags.Abilities_Status_UnLocked;
			bSpendPoint = true;
		}

		// スペルポイントを消費してSpellのレベルをあげる。
		else if (Status.MatchesTagExact(GameplayTags.Abilities_Status_UnLocked) || Status.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
		{
			Spec->Level++;
			bSpendPoint = true;
		}

		if (bSpendPoint && GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPointReward(GetAvatarActor(), -1);
		}

		MarkAbilitySpecDirty(*Spec);
		ClientUpdateAbilityStatuses(AbilityTag, Status, Spec->Level);
	}
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& PrevInputTag, const FGameplayTag& NewInputTag)
{
	OnEquipAbility.Broadcast(AbilityTag, PrevInputTag, NewInputTag);
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatuses_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const int32 AbilityLevel)
{
	OnUpdateAbilityStatuses.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePointReward(GetAvatarActor(), -1.f);
	}
}


void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle) const
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	OnEffectAssetTags.Broadcast(TagContainer);
}
