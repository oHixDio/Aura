// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	if (GetAuraASC()->bStartupAbilitiesGiven)
	{
		BroadcastAbilityInfo();
	}

	OnSpellPointsChanged.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->OnUpdateAbilityStatuses.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const int32 AbilityLevel)
	{
		// セレクトしているアビリティのステータス更新があった場合 || セレクトしているアビリティが
			// Level upしたとき
			// Equipされたとき
			// SpellPointを消費したとき
		if (SelectedSpell.AbilityTag.MatchesTagExact(AbilityTag))
		{
			SelectedSpell.AbilityTag = AbilityTag;
			SelectedSpell.AbilityType = AbilityInfo->FindAbilityForTag(AbilityTag).AbilityTypeTag;
			SelectedSpell.StatusTag = StatusTag;
			ActivateButtonCheck();
		}
		
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityForTag(AbilityTag);
		Info.StatusTag = StatusTag;
		
		AbilityInfoDelegate.Broadcast(Info);
	});

	GetAuraPS()->OnSpellPointsChanged.AddLambda([this](const int32 Points){
		OnSpellPointsChanged.Broadcast(Points);

		if (!SelectedSpell.AbilityTag.MatchesTagExact(FGameplayTag::EmptyTag))
		{
			ActivateButtonCheck();
		}
	});

	GetAuraASC()->OnEquipAbility.AddLambda([this](const FGameplayTag AbilityTag, const FGameplayTag PrevInputTag, const FGameplayTag NewInputTag)
	{
		ActivateButtonCheck();
		
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

		OnStopWaitForEquipSpell.Broadcast(SelectedSpell.AbilityType);
		OnEquipSpell.Broadcast();
	});
}

bool USpellMenuWidgetController::CanSpendPoint() const
{
	return bCanSpendPoint;
}

bool USpellMenuWidgetController::CanEquipSpell() const 
{
	return bCanEquipSpell;
}

FGameplayTag USpellMenuWidgetController::GetSelectedSpellAbilityTag() const
{
	return SelectedSpell.AbilityTag;
}

FGameplayTag USpellMenuWidgetController::GetSelectedSpellAbilityType() const
{
	return SelectedSpell.AbilityType;
}

void USpellMenuWidgetController::SpendSpellPoint(const FGameplayTag& AbilityTag)
{
	if (GetAuraASC())
	{
		GetAuraASC()->ServerSpendSpellPoint(AbilityTag);
	}
}

FSpellInfo USpellMenuWidgetController::GetSpellInfo(const FGameplayTag& AbilityTag)
{
	FSpellInfo Ret = FSpellInfo();
	if (const FGameplayAbilitySpec* Spec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag))
	{
		Ret.AbilityDescriptionInfo = DescriptionInfo->FindAbilityDescriptionForTag(AbilityTag);
		
		const TObjectPtr<UAuraDamageGameplayAbility> Ability = CastChecked<UAuraDamageGameplayAbility>(Spec->Ability);
		FSpellParam Param;
		Param.AbilityName = Ability->GetAbilityName();
		Param.Level = Spec->Level;
		Param.Damage = Ability->GetDamage(Param.Level);
		Param.Cost = FMath::Abs(Ability->GetCost(Param.Level));
		Param.Cooldown = Ability->GetCooldown(Param.Level);
		Param.NextLevel = Param.Level + 1;
		Param.NextDamage = Ability->GetDamage(Param.NextLevel);
		Param.NextCost = FMath::Abs(Ability->GetCost(Param.NextLevel));
		Param.NextCooldown = Ability->GetCooldown(Param.NextLevel);
		Ret.SpellParam = Param;
	}
	return Ret;
}

FGameplayTag USpellMenuWidgetController::GetSpellStatus(const FGameplayTag& AbilityTag)
{
	FGameplayTag StatusTag = FGameplayTag();
	if (const FGameplayAbilitySpec* Spec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag))
	{
		StatusTag = GetAuraASC()->GetStatusTagFromSpec(*Spec);
	}
	return StatusTag;
}

int32 USpellMenuWidgetController::GetLevelRequirement(const FGameplayTag& AbilityTag) const
{
	return AbilityInfo->FindAbilityForTag(AbilityTag).LevelRequirement;
}

void USpellMenuWidgetController::WaitForEquipSpell()
{
	bEquipButtonPressed = true;
	OnWaitForEquipSpell.Broadcast(SelectedSpell.AbilityType);
}

void USpellMenuWidgetController::EquipSpell(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag)
{
	if (GetAuraASC() && bEquipButtonPressed)
	{
		GetAuraASC()->ServerEquipAbility(AbilityTag, InputTag);
		bEquipButtonPressed = false;
	}
}

void USpellMenuWidgetController::SelectSpell(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& AbilityType)
{
	if (bEquipButtonPressed)
	{
		bEquipButtonPressed = false;
		OnStopWaitForEquipSpell.Broadcast(SelectedSpell.AbilityType);
	}

	SelectedSpell.AbilityTag = AbilityTag;
	SelectedSpell.StatusTag = StatusTag;
	SelectedSpell.AbilityType = AbilityType;	

	ActivateButtonCheck();
}

void USpellMenuWidgetController::UnBindAllDelegates()
{
	AbilityInfoDelegate.RemoveAll(this);
	OnSpellPointsChanged.RemoveAll(this);
	OnWaitForEquipSpell.RemoveAll(this);
	OnStopWaitForEquipSpell.RemoveAll(this);
	OnEquipSpell.RemoveAll(this);
}

void USpellMenuWidgetController::ActivateButtonCheck()
{
	const FGameplayTag& StatusTag = SelectedSpell.StatusTag;
	const int32 SpellPoints = GetAuraPS()->GetSpellPoints();
	bCanSpendPoint = !StatusTag.MatchesTag(FAuraGameplayTags::Get().Abilities_Status_Locked) && SpellPoints > 0;
	bCanEquipSpell = StatusTag.MatchesTag(FAuraGameplayTags::Get().Abilities_Status_UnLocked) || StatusTag.MatchesTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
}
