// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WidgetController.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class AURA_API UWidgetController : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category="Galaxy|UI")
	TObjectPtr<APlayerController> PlayerController{};

	UPROPERTY(BlueprintReadOnly, Category="Galaxy|UI")
	TObjectPtr<APlayerState> PlayerState{};

	UPROPERTY(BlueprintReadOnly, Category="Galaxy|UI")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent{};

	UPROPERTY(BlueprintReadOnly, Category="Galaxy|UI")
	TObjectPtr<UAttributeSet> AttributeSet{};
};
