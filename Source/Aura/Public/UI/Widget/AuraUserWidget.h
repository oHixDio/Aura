// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Galaxy|UI")
	TObjectPtr<UObject> WidgetController{};

	UFUNCTION(BlueprintCallable, Category="Galaxy|UI")
	void SetWidgetController(UObject* InWidgetController);

	UFUNCTION(BlueprintImplementableEvent, Category="Galaxy|UI")
	void WidgetControllerSet();
};
