// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	static void GiveCommonAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* AbilitySystemComponent);
};
