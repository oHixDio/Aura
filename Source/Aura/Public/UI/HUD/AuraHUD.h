// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

struct FWidgetControllerParams;
class UOverlayWidgetController;
class UAuraUserWidget;
class UAbilitySystemComponent;
class UAttributeSet;
/**
 * 
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget{};

	UFUNCTION(BlueprintCallable)
	UOverlayWidgetController* GetOverlayWidgetControllerInstance(const FWidgetControllerParams& WidgetControllerParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass{};

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetControllerInstance{};

	UPROPERTY(EditAnywhere, Category = "Galaxy|UI")
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass{};
	
};
