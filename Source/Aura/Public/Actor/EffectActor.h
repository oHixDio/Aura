// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EffectActor.generated.h"

class USphereComponent;

UCLASS()
class AURA_API AEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AEffectActor();

	UFUNCTION(BlueprintCallable)
	void HealthIncrease(AActor* OtherActor, float Value);
	
	UFUNCTION(BlueprintCallable)
	void ManaIncrease(AActor* OtherActor, float Value);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh{};
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere{};
};
