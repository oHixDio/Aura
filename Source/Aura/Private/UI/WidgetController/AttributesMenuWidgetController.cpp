// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributesMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"

void UAttributesMenuWidgetController::BroadcastInitialValues()
{
	for (auto& Pair : GetAuraAS()->TagToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	OnAttributePointsChangedDelegate.Broadcast(GetAuraPS()->GetAttributePoints());
}

void UAttributesMenuWidgetController::BindCallbacksToDependencies()
{
	// TagToAttributesにはMenuに表示する全ての属性が含まれている.
	// そのMapをループして、全ての属性にChangeDelegateを結びつける.
	for (auto& Pair : GetAuraAS()->TagToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this,Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}

	GetAuraPS()->OnAttributePointsChanged.AddLambda(
		[this](const int32 InPoints)
		{
			OnAttributePointsChangedDelegate.Broadcast(InPoints);
		}
	);
}

void UAttributesMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	GetAuraASC()->UpgradeAttribute(AttributeTag);
}

void UAttributesMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	check(AttributeInfo)
	// Infoを渡してWidgetで受け取る。
	// BPで用意したDataAssetに書き込んだ情報がWidgetに伝わる。
	// Widgetでそれらが表示される。
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	OnAttributeInfoDelegate.Broadcast(Info);
}
