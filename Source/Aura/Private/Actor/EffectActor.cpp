// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/EffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
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

void AEffectActor::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// TODO: dirty hack! Debug目的で、constであるべきAttributeSetを強制的に非constにして扱っています.
	if (TScriptInterface<IAbilitySystemInterface> ASIActor = OtherActor)
	{
		const UAuraAttributeSet* AttributeSet = Cast<UAuraAttributeSet>(ASIActor->GetAbilitySystemComponent()->GetAttributeSet(UAuraAttributeSet::StaticClass()));
		UAuraAttributeSet* MutableAttributeSet = const_cast<UAuraAttributeSet*>(AttributeSet);
		MutableAttributeSet->SetHealth(MutableAttributeSet->GetHealth() + 25.f);
		Destroy();
	}
}

void AEffectActor::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
}

void AEffectActor::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AEffectActor::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AEffectActor::OnEndOverlap);
}

