// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();

	UFUNCTION(BlueprintNativeEvent)
	int32 GetPlayerXP() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointReward(const int32 Level) const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointReward(const int32 Level) const;

	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(const int32 InXP);

	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(const int32 InXP);

	UFUNCTION(BlueprintNativeEvent)
	void AddToPlayerLevel(const int32 InLevel);
	
	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePointReward(const int32 InPoint);

	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPointReward(const int32 InPoint);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;
};
