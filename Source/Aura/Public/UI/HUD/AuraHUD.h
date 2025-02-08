// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/WidgetController/AttributesMenuWidgetController.h"
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

	/*
	 * Construction Overlay Widget and Overlay Widget Controller
	 */
public:
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget{};

	UOverlayWidgetController* GetOverlayWidgetControllerInstance(const FWidgetControllerParams& WidgetControllerParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass{};

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetControllerInstance{};

	UPROPERTY(EditAnywhere, Category = "Galaxy|UI")
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass{};


	
	/*
	 * Construction Attributes Menu Widget Controller
	 */
public:
	UAttributesMenuWidgetController* GetAttributesMenuWidgetControllerInstance(const FWidgetControllerParams& WidgetControllerParams);

private:
	UPROPERTY()
	TObjectPtr<UAttributesMenuWidgetController> AttributesMenuWidgetControllerInstance{};

	UPROPERTY(EditAnywhere, Category = "Galaxy|UI")
	TSubclassOf<UAttributesMenuWidgetController> AttributesMenuWidgetControllerClass{};

	
};
