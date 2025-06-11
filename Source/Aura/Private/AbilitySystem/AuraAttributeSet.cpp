// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AuraAbilitySystemFunctionLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Aura/AuraLogChannels.h"
#include "Player/AuraPlayerController.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	FAuraGameplayTags Tags = FAuraGameplayTags::Get();
	TagToAttributes.Add(Tags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagToAttributes.Add(Tags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagToAttributes.Add(Tags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagToAttributes.Add(Tags.Attributes_Primary_Vigor, GetVigorAttribute);
	TagToAttributes.Add(Tags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagToAttributes.Add(Tags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagToAttributes.Add(Tags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	TagToAttributes.Add(Tags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagToAttributes.Add(Tags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagToAttributes.Add(Tags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagToAttributes.Add(Tags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);
	TagToAttributes.Add(Tags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagToAttributes.Add(Tags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagToAttributes.Add(Tags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagToAttributes.Add(Tags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	TagToAttributes.Add(Tags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
	TagToAttributes.Add(Tags.Attributes_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagToAttributes.Add(Tags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
	TagToAttributes.Add(Tags.Attributes_Vital_Health, GetHealthAttribute);
	TagToAttributes.Add(Tags.Attributes_Vital_Mana, GetManaAttribute);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	/** Primary */
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);
	/** Secondary */
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	/** Vital */
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// 変更前の値をクランプするだけ.
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data,Props);

	// 変更後の実際の値をクランプする.
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		// キャッシュして実データは0に戻す.
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);

		// ヘルスを更新する.
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		const bool bDied = NewHealth <= 0.f;

		// 死んでいる.
		if (bDied)
		{
			if (TScriptInterface<ICombatInterface> CombatActor = Props.TargetAvatarActor)
			{
				CombatActor->Die();
			}
			SendXPEvent(Props);
		}
		// 死んでいない.
		else
		{
			// ヒットリアクションAbilityを実行.
			// AbilityのActiveTagでチェック.
			const FGameplayTagContainer TagContainer(FAuraGameplayTags::Get().Events_HitReact);
			Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
		}

		// FloatingDamageテキストを表示する.
		ShowFloatingDamageText(Props, LocalIncomingDamage);
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		const float LocalIncomingXP = GetIncomingXP();
		SetIncomingXP(0.f);
		
		if (Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())
		{
			const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
			const int32 CurrentXP = IPlayerInterface::Execute_GetPlayerXP(Props.SourceCharacter);
			const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter, CurrentXP + LocalIncomingXP);
			const int32 NumLevelUps = NewLevel - CurrentLevel;
		
			if (NumLevelUps > 0)
			{
				const int32 AttributePointReward = IPlayerInterface::Execute_GetAttributePointReward(Props.SourceCharacter, CurrentLevel);
				const int32 SpellPointReward = IPlayerInterface::Execute_GetSpellPointReward(Props.SourceCharacter, CurrentLevel);

				IPlayerInterface::Execute_AddToAttributePointReward(Props.SourceCharacter, AttributePointReward);
				IPlayerInterface::Execute_AddToSpellPointReward(Props.SourceCharacter, SpellPointReward);
				IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter, NumLevelUps);

				bTopOffHealth = true;
				bTopOffMana = true;

				IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);
			}
			
			IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
		}
	}
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute() && bTopOffHealth)
	{
		bTopOffHealth = false;
		SetHealth(GetMaxHealth());
	}

	if (Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		bTopOffMana = false;
		SetMana(GetMaxMana());
	}
}

/** Primary */

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

/** Secondary */

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldFireResistance);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResistance, OldLightningResistance);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldPhysicalResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

/** Vital */

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}



void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	// Source: このEffectの原因, Target: このEffectの対象.
	
	Props.GameplayEffectContextHandle = Data.EffectSpec.GetContext();
	
	// Source.
	Props.SourceASC = Props.GameplayEffectContextHandle.GetOriginalInstigatorAbilitySystemComponent(); // ← このGEの原因となったASCを返却する(チェーンしている場合、本元を返却する).
	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		// Controllerを取得できなければ、AvatarActor経由で取得を試みる.
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Props.SourceController->GetCharacter();
		}
	}

	// Target.
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UAuraAttributeSet::ShowFloatingDamageText(const FEffectProperties& Props, const float DamageValue)
{
	// FloatingDamageテキストを表示する.
	if (Props.TargetCharacter != Props.SourceCharacter)
	{
		// SourceCharacterかTargetCharacterでダメージ表示する.
		// SourceCharacterがEnemyの場合、PlayerControllerへのキャストは叶わない.
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.SourceCharacter->Controller.Get()))
		{
			const bool bIsBlocked = UAuraAbilitySystemFunctionLibrary::IsBlockedHIt(Props.GameplayEffectContextHandle);
			const bool bIsCriticalHit = UAuraAbilitySystemFunctionLibrary::IsCriticalHIt(Props.GameplayEffectContextHandle);
			PC->ClientShowFloatingDamage(DamageValue, Props.TargetCharacter, bIsBlocked, bIsCriticalHit);
			return;
		}
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.TargetCharacter->Controller.Get()))
		{
			const bool bIsBlocked = UAuraAbilitySystemFunctionLibrary::IsBlockedHIt(Props.GameplayEffectContextHandle);
			const bool bIsCriticalHit = UAuraAbilitySystemFunctionLibrary::IsCriticalHIt(Props.GameplayEffectContextHandle);
			PC->ClientShowFloatingDamage(DamageValue, Props.TargetCharacter, bIsBlocked, bIsCriticalHit);
		}
	}
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& Props) const
{ 
	if (Props.TargetCharacter->Implements<UCombatInterface>())
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);
		const ECharacterClass TargetCharacterClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);

		const int32 XPReward = UAuraAbilitySystemFunctionLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter, TargetCharacterClass, TargetLevel);
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.Attributes_Meta_IncomingXP;
		Payload.EventMagnitude = XPReward;
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attributes_Meta_IncomingXP, Payload);
	}
}
