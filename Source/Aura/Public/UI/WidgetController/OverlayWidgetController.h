// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/WidgetController.h"
#include "OverlayWidgetController.generated.h"

struct FAuraAbilityInfo;
class UAuraAbilitySystemComponent;
class UAuraUserWidget;
struct FOnAttributeChangeData;
class UAbilityInfo;

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag Tag{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> WidgetClass{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Texture{};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeValueChanged, float, NewValue);
/**  */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowDelegate, FUIWidgetRow, Row);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAuraAbilityInfo&, Info);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class AURA_API UOverlayWidgetController : public UWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="Aura|Event")
	FOnAttributeValueChanged OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category="Aura|Event")
	FOnAttributeValueChanged OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Aura|Event")
	FOnAttributeValueChanged OnManaChanged;
	
	UPROPERTY(BlueprintAssignable, Category="Aura|Event")
	FOnAttributeValueChanged OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category="Aura|Event")
	FMessageWidgetRowDelegate OnMessageWidgetRowDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Event")
	FAbilityInfoSignature AbilityInfoDelegate;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UDataTable> MessageWidgetDataTable{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAbilityInfo> AbilityInfo{};

	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag) const;

	void OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraASC) const;
	
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag) const
{
	return DataTable->FindRow<FUIWidgetRow>(Tag.GetTagName(), TEXT(""));
}
