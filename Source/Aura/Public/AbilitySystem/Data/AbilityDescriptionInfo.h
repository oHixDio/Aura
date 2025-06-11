// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityDescriptionInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraAbilityDescriptionInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Multiline = "true"))
	FString Description = FString();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Multiline = "true"))
	FString NextLevelDescription = FString();
};


/**
 * 
 */
UCLASS()
class AURA_API UAbilityDescriptionInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraAbilityDescriptionInfo> AbilityDescriptionInformation = {};

	FAuraAbilityDescriptionInfo FindAbilityDescriptionForTag(const FGameplayTag& AbilityTag, bool bLogNotFound = false) const;
};
