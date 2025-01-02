// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WidgetController.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams() {}
	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
		: PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet{};
};

/**
 * 
 */
UCLASS()
class AURA_API UWidgetController : public UObject
{
	GENERATED_BODY()

public:
	void SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams);

	virtual void BroadcastInitialValues();
	virtual void BindCallbacksToDependencies();
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
