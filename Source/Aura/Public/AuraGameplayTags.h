// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
/**
 * Singleton クラスです。
 * Native GameplayTags にアクセスできます。
 */
struct FAuraGameplayTags
{
public:
	static const FAuraGameplayTags& Get() { return Instance; };
	
	static void InitializeNativeGameplayTags();

	/**
	 * Primary Attributes
	 */
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;

	/**
	 * Secondary Attributes
	 */
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;

	/**
	 * Vital Attributes
	 */
	FGameplayTag Attributes_Vital_Health;
	FGameplayTag Attributes_Vital_Mana;

	/**
	* Input
	*/
	FGameplayTag Input_1;
	FGameplayTag Input_2;
	FGameplayTag Input_3;
	FGameplayTag Input_4;
	FGameplayTag Input_LMB;
	FGameplayTag Input_RMB;

	FGameplayTag Damage;

	/**
	* Event 
	*/
	FGameplayTag Events_HitReact;
	
private:
	static FAuraGameplayTags Instance;
	
};
