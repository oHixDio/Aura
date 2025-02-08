// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "EffectActor.generated.h"


class UGameplayEffect;
class UAbilitySystemComponent;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	DoNotApply,
	ApplyOnBeginOverlap,
	ApplyOnEndOverlap
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
	DoNotRemoval,
	RemoveOnEndOverlap
};

UCLASS()
class AURA_API AEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AEffectActor();

protected:
	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	UFUNCTION(BlueprintCallable)
	void OnBeginOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Galaxy| Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Galaxy| Effects")
	EEffectApplicationPolicy InstantApplicationPolicy{ EEffectApplicationPolicy::DoNotApply };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Galaxy| Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Galaxy| Effects")
	EEffectApplicationPolicy DurationApplicationPolicy{ EEffectApplicationPolicy::DoNotApply };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Galaxy| Effects")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Galaxy| Effects")
	EEffectApplicationPolicy InfiniteApplicationPolicy{ EEffectApplicationPolicy::DoNotApply };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Galaxy| Effects")
	EEffectRemovalPolicy InfiniteRemovalPolicy{ EEffectRemovalPolicy::RemoveOnEndOverlap };

	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Galaxy| Effects")
	float ActorLevel{ 1.f };
};
