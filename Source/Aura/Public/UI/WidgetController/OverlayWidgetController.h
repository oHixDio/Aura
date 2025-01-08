// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/WidgetController.h"
#include "OverlayWidgetController.generated.h"

class UAuraUserWidget;
struct FOnAttributeChangeData;

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

	UPROPERTY(BlueprintAssignable, Category="Galaxy|Event")
	FOnAttributeValueChanged OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category="Galaxy|Event")
	FOnAttributeValueChanged OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Galaxy|Event")
	FOnAttributeValueChanged OnManaChanged;
	
	UPROPERTY(BlueprintAssignable, Category="Galaxy|Event")
	FOnAttributeValueChanged OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category="Galaxy|Event")
	FMessageWidgetRowDelegate OnMessageWidgetRowDelegate;
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UDataTable> MessageWidgetDataTable{};

	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag) const;
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag) const
{
	return DataTable->FindRow<FUIWidgetRow>(Tag.GetTagName(), TEXT(""));
}
