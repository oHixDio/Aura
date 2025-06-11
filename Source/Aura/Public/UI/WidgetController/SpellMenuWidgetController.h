// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Data/AbilityDescriptionInfo.h"
#include "UI/WidgetController/WidgetController.h"
#include "SpellMenuWidgetController.generated.h"


class UAbilityDescriptionInfo;

USTRUCT(BlueprintType)
struct FSpellParam
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FText AbilityName = FText();

	UPROPERTY(BlueprintReadOnly)
	int32 Level = 0;

	UPROPERTY(BlueprintReadOnly)
	float Damage = 0;

	UPROPERTY(BlueprintReadOnly)
	float Cost = 0;
	
	UPROPERTY(BlueprintReadOnly)
	float Cooldown = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 NextLevel = 0;

	UPROPERTY(BlueprintReadOnly)
	float NextDamage = 0;

	UPROPERTY(BlueprintReadOnly)
	float NextCost = 0;
	
	UPROPERTY(BlueprintReadOnly)
	float NextCooldown = 0;
};

USTRUCT(BlueprintType)
struct FSpellInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FAuraAbilityDescriptionInfo AbilityDescriptionInfo = FAuraAbilityDescriptionInfo();

	UPROPERTY(BlueprintReadOnly)
	FSpellParam SpellParam = FSpellParam();
};

USTRUCT(BlueprintType)
struct FSelectedSpell
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag AbilityTag = FGameplayTag::EmptyTag;
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag StatusTag = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag AbilityType = FGameplayTag::EmptyTag;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaitForEquipSpell, const FGameplayTag,  AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipSpell);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class AURA_API USpellMenuWidgetController : public UWidgetController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSigneture OnSpellPointsChanged;

	UPROPERTY(BlueprintAssignable)
	FOnWaitForEquipSpell OnWaitForEquipSpell;

	UPROPERTY(BlueprintAssignable)
	FOnWaitForEquipSpell OnStopWaitForEquipSpell;

	UPROPERTY(BlueprintAssignable)
	FOnEquipSpell OnEquipSpell;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAbilityDescriptionInfo> DescriptionInfo = nullptr;
	
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanSpendPoint() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanEquipSpell() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetSelectedSpellAbilityTag() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetSelectedSpellAbilityType() const;

	UFUNCTION(BlueprintCallable)
	void SpendSpellPoint(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FSpellInfo GetSpellInfo(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetSpellStatus(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetLevelRequirement(const FGameplayTag& AbilityTag) const;

	UFUNCTION(BlueprintCallable)
	void WaitForEquipSpell();
	
	UFUNCTION(BlueprintCallable)
	void EquipSpell(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable)
	void SelectSpell(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& AbilityType);

	UFUNCTION(BlueprintCallable)
	void UnBindAllDelegates();
	
private:
	bool bEquipButtonPressed = false;

	bool bCanSpendPoint = false;

	bool bCanEquipSpell = false;

	FSelectedSpell SelectedSpell = FSelectedSpell();

	void ActivateButtonCheck();
};
