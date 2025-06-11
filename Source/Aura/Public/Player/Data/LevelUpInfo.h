// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraLevelUpInfo
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 LevelUpRequirement = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AttributePointReward = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpellPointReward = 1;
};

/**
 * 
 */
UCLASS()
class AURA_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FAuraLevelUpInfo> LevelUpInformation = {};

	UFUNCTION(BlueprintCallable)
	int32 FindLevelForXP(const int32 XP) const;
};
