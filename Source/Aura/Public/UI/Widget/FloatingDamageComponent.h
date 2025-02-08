// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "FloatingDamageComponent.generated.h"

USTRUCT(BlueprintType)
struct FDamageTypes
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float Damage{0.f};

	UPROPERTY(BlueprintReadOnly)
	bool bIsBlocked{false};
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsCriticalHit{false};
};

/**
 * 
 */
UCLASS()
class AURA_API UFloatingDamageComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageTypes(const FDamageTypes DamageTypes);

};
