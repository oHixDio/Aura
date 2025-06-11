// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AuraPlayerState.generated.h"

class ULevelUpInfo;
class UAttributeSet;
class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32);

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	FOnPlayerStatChanged OnLevelChanged;
	
	FOnPlayerStatChanged OnXPChanged;

	FOnPlayerStatChanged OnAttributePointsChanged;

	FOnPlayerStatChanged OnSpellPointsChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo{};
	
	AAuraPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UAttributeSet* GetAttributeSet() const
	{
		return AttributeSet;
	}

	FORCEINLINE int32 GetPlayerLevel() const
	{
		return Level;
	}

	FORCEINLINE int32 GetXP() const
	{
		return XP;
	}

	FORCEINLINE int32 GetAttributePoints() const
	{
		return AttributePoints;
	}

	FORCEINLINE int32 GetSpellPoints() const
	{
		return SpellPoints;
	}

	void SetLevel(const int32 InLevel);

	void SetXP(const int32 InXP);

	void SetAttributePoints(const int32 InAttributePoints);

	void SetSpellPoints(const int32 InSpellPoints);

	UFUNCTION(BlueprintCallable)
	void AddToLevel(const int32 InLevel);

	UFUNCTION(BlueprintCallable)
	void AddToXP(const int32 InXP);

	UFUNCTION(BlueprintCallable)
	void AddToAttributePoints(const int32 InAttributePoints);

	UFUNCTION(BlueprintCallable)
	void AddToSpellPoints(const int32 InSpellPoints);
	
protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing= OnRep_Level)
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing= OnRep_XP)
	int32 XP = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing= OnRep_AttributePoints)
	int32 AttributePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing= OnRep_SpellPoints)
	int32 SpellPoints = 0;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel) const;
	
	UFUNCTION()
	void OnRep_XP(int32 OldXP) const;

	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints) const;

	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints) const;
};
