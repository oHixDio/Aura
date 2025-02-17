// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

struct FGameplayTag;
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura")
	ECharacterClass CharacterClass{ECharacterClass::Warrior};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura")
	float BaseWalkSpeed{250.f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura")
	float BaseRotationSpeedYaw{360.f};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh{};

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent{};

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet{};

	virtual void InitAbilityActorInfo();


	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// CombatInterface member.
public:
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& SocketTag) const override;

	virtual UAnimMontage* GetHitReactMontage_Implementation() const override;

	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() const override;

	virtual FTaggedMontage GetAttackMontageByMontageTag_Implementation(const FGameplayTag& MontageTag) const override;

	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;

	virtual void Die() override;

	/*
	 * 死亡時のServer,Clientの共通処理を記述.
	 */
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastDie();
	
	virtual bool IsDead_Implementation() const override;

protected:
	UPROPERTY(EditAnywhere, Category = "Aura")
	TObjectPtr<UAnimMontage> HitReactMontage{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura")
	TArray<FTaggedMontage> TaggedMontages{};

	UPROPERTY(EditAnywhere, Category = "Aura")
	TObjectPtr<UNiagaraSystem> BloodEffect{};

	UPROPERTY(EditAnywhere, Category = "Aura")
	TObjectPtr<USoundBase> DeathSound{};

	bool bIsDead{false};
	
	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// Initialize Attributes member.
public:
	UPROPERTY(EditAnywhere, Category = "Aura")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributesClass;

	UPROPERTY(EditAnywhere, Category = "Aura")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributesClass;

	UPROPERTY(EditAnywhere, Category = "Aura")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributesClass;

	virtual void InitializeDefaultAttributes() const;

	

	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// Add StartUp Abilities member.
protected:
	/** StartupAbilitiesをキャラクターに付与する. */
	void AddCharacterAbilities() const;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Aura")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;


	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// Dissolve member.
protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Aura")
	TObjectPtr<UMaterialInstance> MeshDissolveMaterial{};

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Aura")
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterial{};

	UFUNCTION(BlueprintImplementableEvent)
	void StartMeshDissolveTimeLine(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeLine(UMaterialInstanceDynamic* DynamicMaterialInstance);

private:
	void Dissolve();


	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// HitReact member.
public:
	virtual void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
protected:
	
	UPROPERTY(BlueprintReadOnly, Category = "Aura")
	bool bHitReacting{false};
	
};
