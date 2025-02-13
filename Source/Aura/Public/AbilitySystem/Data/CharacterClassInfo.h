// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Elementalist, Warrior, Ranger,
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Class Default")
	TSubclassOf<UGameplayEffect> PrimaryAttributesEffectClass{};

	UPROPERTY(EditDefaultsOnly, Category="Class Default")
	TArray<TSubclassOf<UGameplayAbility>> Abilities{};
};

/**
 * 
 */
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Character Class Default")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation{};
	
	UPROPERTY(EditDefaultsOnly, Category="Common Class Default")
	TSubclassOf<UGameplayEffect> SecondaryAttributesEffectClass{};
	
	UPROPERTY(EditDefaultsOnly, Category="Common Class Default")
	TSubclassOf<UGameplayEffect> VitalAttributesEffectClass{};

	UPROPERTY(EditDefaultsOnly, Category="Common Class Default")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities{};

	UPROPERTY(EditDefaultsOnly, Category="Common Class Default")
	TObjectPtr<UCurveTable> DamageCalculationCoefficientCurveTable{};
	
	FCharacterClassDefaultInfo GetClassDefaultInfo(const ECharacterClass CharacterClass) const;
	
};
