// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemFunctionLibrary.generated.h"

class UOverlayWidgetController;
class UAttributesMenuWidgetController;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "AbilitySystemFunctionLibrary|WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AbilitySystemFunctionLibrary|WidgetController")
	static UAttributesMenuWidgetController* GetAttributesMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "AbilitySystemFunctionLibrary|Character")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, const ECharacterClass CharacterClass, const float Level, UAbilitySystemComponent* AbilitySystemComponent);

	UFUNCTION(BlueprintCallable, Category = "AbilitySystemFunctionLibrary|Character")
	static void GiveDefaultAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* AbilitySystemComponent, const ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category = "AbilitySystemFunctionLibrary|DamageCalcuation")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AbilitySystemFunctionLibrary|GameplayEffects")
	static bool IsBlockedHIt(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AbilitySystemFunctionLibrary|GameplayEffects")
	static bool IsCriticalHIt(const FGameplayEffectContextHandle& EffectContextHandle);

	// 非const型はBPでは出力となってしまいます。
	// UPARAM(Ref) と記述することで、入力にします。
	UFUNCTION(BlueprintCallable, Category = "AbilitySystemFunctionLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(Ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlocked);
	
	UFUNCTION(BlueprintCallable, Category = "AbilitySystemFunctionLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(Ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

	UFUNCTION(BlueprintCallable, Category = "AbilitySystemFunctionLibrary|GameplayMechanics")
	static void GetLivePlayersWithInRadius(const UObject* WorldContextObject, const float Radius, TArray<AActor*>& OutActors, const TArray<AActor*>& IgnoreActors, const FVector& RadialOrigin);

	UFUNCTION(BlueprintPure, Category = "AbilitySystemFunctionLibrary|GameplayEffects")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);
};

