// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"
#include "Player/Data/LevelUpInfo.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());

	if (GetAuraASC()->bStartupAbilitiesGiven)
	{
		BroadcastAbilityInfo();
	}
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->OnXPChanged.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetAuraPS()->OnLevelChanged.AddLambda(
		[this] (const int32 NewLevel)
		{
			OnPlayerLevelChanged.Broadcast(NewLevel);
		}
	);

	GetAuraASC()->OnEquipAbility.AddLambda([this](const FGameplayTag AbilityTag, const FGameplayTag PrevInputTag, const FGameplayTag NewInputTag)
	{
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		FAuraAbilityInfo Prev = {};
		Prev.AbilityTag = FGameplayTag::EmptyTag;
		Prev.StatusTag = GameplayTags.Abilities_Status_UnLocked;
		Prev.InputTag = PrevInputTag;
		AbilityInfoDelegate.Broadcast(Prev);
		
		FAuraAbilityInfo New = AbilityInfo->FindAbilityForTag(AbilityTag);
		New.AbilityTag = AbilityTag;
		New.StatusTag = GameplayTags.Abilities_Status_Equipped;
		New.InputTag = NewInputTag;
		AbilityInfoDelegate.Broadcast(New);
	});
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxHealthAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		// StartupAbilitiesが付与された後なら直接呼び出す。
		if (AuraASC->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		else
		{
			// StartupAbilitiesが付与される前ならStartupAbilities完了イベントにバインドする。
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &ThisClass::BroadcastAbilityInfo);
		}
		
		GetAuraASC()->OnEffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (FGameplayTag Tag : AssetTags)
				{
					// "Message.PotionHealth" MatchesTag("Message") return TRUE, "Message" MatchesTag("Message.PotionHealth") return FALSE.
					if (const FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message")); Tag.MatchesTag(MessageTag))
					{
						/**
						 * ・ GameEffectはAssetTagsを持つ.
						 *		- デリゲード通知でActive情報を受け渡し.
						 * ・ AbilitySystemComponentがActiveGEをキャッチ.
						 *		- デリゲード通知でActiveGEが持つAssetTagsを受け渡し.
						 * ・ WidgetControllerがAssetTagsをキャッチ.
						 *		- デリゲード通知でAssetTagsから取得したUIWidgetRowを受け渡し.	← イマココ.
						 * ・ Widget
						 */
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						OnMessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		);
	}
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const ULevelUpInfo* LevelUpInfo = GetAuraPS()->LevelUpInfo;
	check(LevelUpInfo);

	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();
	if (0 < Level && Level < MaxLevel)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;

		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

		const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);

		OnXPPercentChanged.Broadcast(XPBarPercent);
	}
}
