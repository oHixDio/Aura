// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "Interaction/Highlightable.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class AAuraAIController;
class UAuraUserWidget;
class UEnemyWidgetController;
class UWidgetComponent;
class UBehaviorTree;
/**
 * 
 */
UCLASS(BlueprintType)
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IHighlightable, public IEnemyInterface
{
	GENERATED_BODY()

	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// Super member.
public:
	AAuraEnemy();

	virtual void PossessedBy(AController* NewController) override;
	
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// ====== ====== ====== ====== ====== ====== 
	// Core member.
protected:
	virtual void InitAbilityActorInfo() override;
	
	virtual void InitializeDefaultAttributes() const override;
	
	// ====== ====== ====== ====== ====== ====== 
	// HighlightableInterface member.
public:
	UPROPERTY(BlueprintReadWrite, Category = "Aura")
	bool bHighlighted{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura")
	float LifeSpan{5.f};
	
	virtual void Highlight() override;
	virtual void UnHighlight() override;

	// ====== ====== ====== ====== ====== ====== 
	// CombatInterface member.
public:
	virtual float GetPlayerLevel() const override;

	virtual void Die() override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura")
	float Level{1.f};

	

	// ====== ====== ====== ====== ====== ====== 
	// EnemyInterface member.
public:
	virtual AActor* GetCombatTarget_Implementation() override;

	virtual void SetCombatTarget_Implementation(AActor* Target) override;

	virtual int32 GetMinionCount_Implementation() override;

	virtual void IncrementMinionCount_Implementation() override;

	virtual void DecrementMinionCount_Implementation() override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Aura")
	TObjectPtr<AActor> CombatTarget{};

	int32 MinionCount{};


	// ====== ====== ====== ====== ====== ====== 
	// HealthBar member.
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBarWidget{};

	UPROPERTY(BlueprintAssignable)
	FOnAttributeValueChanged OnHealthValueChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeValueChanged OnMaxHealthValueChanged;

public:
	virtual void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;

	// ====== ====== ====== ====== ====== ====== 
	// AI member.
protected:
	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura")
	TObjectPtr<UBehaviorTree> BehaviorTree;
};
