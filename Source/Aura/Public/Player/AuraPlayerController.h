// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "Interaction/Highlightable.h"
#include "AuraPlayerController.generated.h"

class USplineComponent;
class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
class UAuraAbilitySystemComponent;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// Super member.
public:
	AAuraPlayerController();

	virtual void PlayerTick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;



	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// Core member.
public:
	UAuraAbilitySystemComponent* GetASC();

private:
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent{};

	
	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// Input member.
private:
	UPROPERTY(EditAnywhere, Category = "Aura|Input")
	TObjectPtr<UInputMappingContext> AuraContext{};

	UPROPERTY(EditAnywhere, Category = "Aura|Input")
	TObjectPtr<UInputAction> MoveAction{};

	UPROPERTY(EditAnywhere, Category = "Aura|Input")
	TObjectPtr<UAuraInputConfig> InputConfig{};

	void Move(const FInputActionValue& InputActionValue);

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);



	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// HighLight member.
private:
	TScriptInterface<IHighlightable> LastHoverActor{};
	TScriptInterface<IHighlightable> ThisHoverActor{};

	FHitResult CursorHit;
	
	void CursorTrace();


	
	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// Click to move member.
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline{};

	/** 現在位置から目的地までの距離が、この値を下回ったら自動Runを終了する. */
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius{50.f};

	float FollowTime{};
	float ShortPressThreshold{0.5f};
	FVector CashedDestination{};
	bool bAutoRunning{false};
	/** EnemyなどのTarget対象にカーソルがホバーしているか否かを判定. */
	bool bTargeting{false};

	void AutoRun();
};
