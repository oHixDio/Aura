// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UGameplayAbility;
class UAttributeSet;
class UAbilitySystemComponent;
class UGameplayEffect;

UCLASS()
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// Super member.
public:
	AAuraCharacterBase();

protected:
	virtual void BeginPlay() override;


	
	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// Core member.
public:
	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& EffectClass, const float Level) const;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	virtual FVector GetCombatSocketLocation() const override;
	
protected:
	virtual void InitAbilityActorInfo();

	UPROPERTY(EditAnywhere, Category = "Aura|Character")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh{};

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, Category = "Aura|Character")
	FName WeaponTipSocketName{};


	
	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// Initialize Attributes member.
public:
	UPROPERTY(EditAnywhere, Category = "Aura|Character")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributesClass;

	UPROPERTY(EditAnywhere, Category = "Aura|Character")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributesClass;

	UPROPERTY(EditAnywhere, Category = "Aura|Character")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributesClass;

	virtual void InitializeDefaultAttributes() const;

	

	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// Initialize Abilities member.
protected:
	/** StartupAbilitiesをキャラクターに付与する. */
	void AddCharacterAbilities() const;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "CharacterBase")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};
