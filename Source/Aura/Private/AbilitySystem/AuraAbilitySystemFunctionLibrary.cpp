// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemFunctionLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

UOverlayWidgetController* UAuraAbilitySystemFunctionLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* HUD = PC->GetHUD<AAuraHUD>())
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return HUD->GetOverlayWidgetControllerInstance(WidgetControllerParams);
		}
	}

	return nullptr;
}

UAttributesMenuWidgetController* UAuraAbilitySystemFunctionLibrary::GetAttributesMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* HUD = PC->GetHUD<AAuraHUD>())
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);

			return HUD->GetAttributesMenuWidgetControllerInstance(WidgetControllerParams);
		}
	}

	return nullptr;
}
