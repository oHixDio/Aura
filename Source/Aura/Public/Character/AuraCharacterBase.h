// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UWidgetComponent;
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

protected:
	UPROPERTY(EditAnywhere, Category = "Aura|Character")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh{};

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent{};

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet{};

	virtual void InitAbilityActorInfo();


	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// CombatInterface member.
public:
	virtual FVector GetCombatSocketLocation() const override;

	virtual UAnimMontage* GetHitReactMontage_Implementation() const override;

	virtual void Die() override;

	/*
	 * 死亡時のServer,Clientの共通処理を記述.
	 */
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastDie();

protected:
	UPROPERTY(EditAnywhere, Category = "Aura|Character")
	FName WeaponTipSocketName{};

	UPROPERTY(EditAnywhere, Category = "Aura|Character")
	TObjectPtr<UAnimMontage> HitReactMontage{};
	
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
	// Add Abilities member.
protected:
	/** StartupAbilitiesをキャラクターに付与する. */
	void AddCharacterAbilities() const;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Aura|Character")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;


	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// Dissolve member.
protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Aura|Character")
	TObjectPtr<UMaterialInstance> MeshDissolveMaterial{};

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Aura|Character")
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterial{};

	UFUNCTION(BlueprintImplementableEvent)
	void StartMeshDissolveTimeLine(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeLine(UMaterialInstanceDynamic* DynamicMaterialInstance);

private:
	void Dissolve();
};
