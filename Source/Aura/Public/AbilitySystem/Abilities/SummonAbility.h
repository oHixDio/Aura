// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "SummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();

	UFUNCTION(BlueprintPure)
	TSubclassOf<APawn> GetRandomSpawnMinion() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<APawn>> MinionClasses{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 NumOfMinions{3};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SpawnSpread{90.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SpawnMinDistance{200.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SpawnMaxDistance{500.f};
};
