// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetController.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
class AAuraPlayerController;
class AAuraPlayerState;
class UAuraAbilitySystemComponent;
class UAuraAttributeSet;
class UAbilityInfo;
struct FAuraAbilityInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSigneture, int32, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAuraAbilityInfo&, Info);

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
UCLASS(Blueprintable, BlueprintType)
class AURA_API UWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Aura|Event")
	FAbilityInfoSignature AbilityInfoDelegate;
	
	void SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams);

	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues() { };
	virtual void BindCallbacksToDependencies() { };
	void BroadcastAbilityInfo();
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAbilityInfo> AbilityInfo = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category="Aura|UI")
	TObjectPtr<APlayerController> PlayerController{};

	UPROPERTY(BlueprintReadOnly, Category="Aura|UI")
	TObjectPtr<APlayerState> PlayerState{};

	UPROPERTY(BlueprintReadOnly, Category="Aura|UI")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent{};

	UPROPERTY(BlueprintReadOnly, Category="Aura|UI")
	TObjectPtr<UAttributeSet> AttributeSet{};

	AAuraPlayerController* GetAuraPC();
	
	AAuraPlayerState* GetAuraPS();
	
	UAuraAbilitySystemComponent* GetAuraASC();
	
	UAuraAttributeSet* GetAuraAS();

private:
	UPROPERTY()
	TObjectPtr<AAuraPlayerController> AuraPlayerController{};

	UPROPERTY()
	TObjectPtr<AAuraPlayerState> AuraPlayerState{};

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent{};

	UPROPERTY()
	TObjectPtr<UAuraAttributeSet> AuraAttributeSet{};
};
