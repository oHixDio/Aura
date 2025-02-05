// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/WidgetController.h"
#include "EnemyWidgetController.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthValueChanged, float, NewValue);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class AURA_API UEnemyWidgetController : public UWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="Galaxy|Event")
	FOnHealthValueChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Galaxy|Event")
	FOnHealthValueChanged OnMaxHealthChanged;
};
