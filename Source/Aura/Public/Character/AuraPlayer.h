// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UNiagaraComponent;
class UWidgetComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayer : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// Super member.
public:
	AAuraPlayer();
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void OnRep_PlayerState() override;

	virtual void InitializeDefaultAttributes() const override;

	// ~~ ICombatInterface Implementation
	virtual float GetPlayerLevel_Implementation() const override;
	// ~~

	// ~~ IPlayerInterface Implementation
	virtual void LevelUp_Implementation() override;
	virtual int32 GetPlayerXP_Implementation() const override;
	virtual int32 GetAttributePointReward_Implementation(const int32 Level) const override;
	virtual int32 GetSpellPointReward_Implementation(const int32 Level) const override;
	virtual int32 FindLevelForXP_Implementation(const int32 InXP) override;
	virtual void AddToXP_Implementation(const int32 InXP) override;
	virtual void AddToPlayerLevel_Implementation(const int32 InLevel) override;
	virtual void AddToAttributePointReward_Implementation(const int32 InPoint) override;
	virtual void AddToSpellPointReward_Implementation(const int32 InPoint) override;
	virtual int32 GetAttributePoints_Implementation() const override;
	// ~~
	
protected:
	virtual void InitAbilityActorInfo() override;

	virtual void BeginPlay() override;


	
	// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
	// Core member.
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura")
	bool bTakeHitReact{false};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aura|Component")
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aura|Component")
	TObjectPtr<UCameraComponent> TopDownCameraComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Aura|LevelUp")
	TSubclassOf<UWidgetComponent> FloatingLevelUpWidgetClass = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aura|LevelUp")
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraSystemComponent = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Aura|LevelUp")
	TObjectPtr<USoundBase> LevelUpSound = nullptr;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpSound();

	UFUNCTION(Client, Reliable)
	void ClientShowLevelUpMessage();
};
