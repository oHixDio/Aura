// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/Highlightable.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UAuraUserWidget;
class UEnemyWidgetController;
class UWidgetComponent;
/**
 * 
 */
UCLASS(BlueprintType)
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IHighlightable
{
	GENERATED_BODY()

	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// Super member.
public:
	AAuraEnemy();
	
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// ====== ====== ====== ====== ====== ====== 
	// Core member.
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Character")
	ECharacterClass CharacterClass{ECharacterClass::Warrior};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Character")
	float BaseWalkSpeed{250.f};

	virtual void InitAbilityActorInfo() override;
	
	virtual void InitializeDefaultAttributes() const override;
	
	// ====== ====== ====== ====== ====== ====== 
	// CombatInterface member.
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHighlighted{false};
	
	virtual void Highlight() override;
	virtual void UnHighlight() override;

	virtual float GetPlayerLevel() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Character")
	float Level{1.f};

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
	UPROPERTY(BlueprintReadOnly, Category = "Aura|Character")
	bool bHitReacting{false};
	
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
