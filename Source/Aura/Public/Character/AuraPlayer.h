// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraPlayer.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayer : public AAuraCharacterBase
{
	GENERATED_BODY()

public:
	AAuraPlayer();
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void OnRep_PlayerState() override;

	virtual float GetPlayerLevel() const override;

	virtual void InitializeDefaultAttributes() const;
	
protected:
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura")
	bool bTakeHitReact{false};
};
