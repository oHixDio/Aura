// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/EffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/SphereComponent.h"

AEffectActor::AEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());
}

void AEffectActor::HealthIncrease(AActor* OtherActor, float Value)
{
	if (TScriptInterface<IAbilitySystemInterface> ASIActor = OtherActor)
	{
		const UAuraAttributeSet* AttributeSet = Cast<UAuraAttributeSet>(ASIActor->GetAbilitySystemComponent()->GetAttributeSet(UAuraAttributeSet::StaticClass()));
		UAuraAttributeSet* MutableAttributeSet = const_cast<UAuraAttributeSet*>(AttributeSet);
		MutableAttributeSet->SetHealth(MutableAttributeSet->GetHealth() + Value);
		Destroy();
	}
}

void AEffectActor::ManaIncrease(AActor* OtherActor, float Value)
{
	if (TScriptInterface<IAbilitySystemInterface> ASIActor = OtherActor)
	{
		const UAuraAttributeSet* AttributeSet = Cast<UAuraAttributeSet>(ASIActor->GetAbilitySystemComponent()->GetAttributeSet(UAuraAttributeSet::StaticClass()));
		UAuraAttributeSet* MutableAttributeSet = const_cast<UAuraAttributeSet*>(AttributeSet);
		MutableAttributeSet->SetMana(MutableAttributeSet->GetMana() + Value);
		Destroy();
	}
}