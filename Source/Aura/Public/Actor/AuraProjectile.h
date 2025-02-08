// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UNiagaraSystem;

UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraProjectile();

	FGameplayEffectSpecHandle DamageEffectSpecHandle{};

protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere{};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement{};

	UFUNCTION()
	void OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditDefaultsOnly)
	float ProjectileLifeSpan{15.f};
	
	bool bHit{false};
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect{};

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound{};

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> TrailSound{};

	UPROPERTY()
	TObjectPtr<UAudioComponent> TrailAudio{};
};
