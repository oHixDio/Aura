// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/Highlightable.h"
#include "AuraEnemy.generated.h"

class UAuraUserWidget;
class UEnemyWidgetController;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IHighlightable
{
	GENERATED_BODY()

public:
	AAuraEnemy();
	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHighlighted{false};
	
	virtual void Highlight() override;
	virtual void UnHighlight() override;

	virtual float GetPlayerLevel() const override;
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Level{1.f};

	UPROPERTY()
	TObjectPtr<UEnemyWidgetController> EnemyWidgetController{};

	UPROPERTY(EditAnywhere)
	TSubclassOf<UEnemyWidgetController> EnemyWidgetControllerClass{};

	UFUNCTION(BlueprintCallable)
	UEnemyWidgetController* GetEnemyWidgetController();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBarWidget{};

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> HealthBarWidgetClass{};

private:
	void InitEnemyWidget();
};
