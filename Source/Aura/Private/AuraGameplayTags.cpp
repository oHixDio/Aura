// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"


FAuraGameplayTags FAuraGameplayTags::Instance;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	/**
	 * Primary Attributes
	 */
	Instance.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Strength"));
	Instance.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Intelligence"));
	Instance.Attributes_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Resilience"));
	Instance.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Vigor"));
	
	/**
	 * Secondary Attributes
	 */
	Instance.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxHealth"));
	Instance.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxMana"));
	Instance.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Armor"));
	Instance.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.ArmorPenetration"));
	Instance.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.BlockChance"));
	Instance.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitChance"));
	Instance.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitDamage"));
	Instance.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitResistance"));
	Instance.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.HealthRegeneration"));
	Instance.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.ManaRegeneration"));

	/**
	 * Vital Attributes
	 */
	Instance.Attributes_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Vital.Health"));
	Instance.Attributes_Vital_Mana = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Vital.Mana"));

	/**
	 * Input
	 */
	Instance.Input_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.1"), FString("Input Tag for 1 key"));
	Instance.Input_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.2"), FString("Input Tag for 2 key"));
	Instance.Input_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.3"), FString("Input Tag for 3 key"));
	Instance.Input_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.4"), FString("Input Tag for 4 key"));
	Instance.Input_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.LMB"), FString("Input Tag for Left mouse button"));
	Instance.Input_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.RMB"), FString("Input Tag for Right mouse button"));

	/**
	 * Damage
	 */
	Instance.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage"), FString("Damage"));

	/**
	* Event 
	*/
	Instance.Events_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Events.HitReact"), FString("Hit reaction by damage"));
}
