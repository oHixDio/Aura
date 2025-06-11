// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"


AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	
	SetNetUpdateFrequency(100.f);
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, XP);
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::SetLevel(const int32 InLevel)
{
	Level = InLevel;
	Level = FMath::Max(Level, 0);
	OnLevelChanged.Broadcast(Level);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AuraASC->UpdateAbilityStatuses(Level);
	}
}

void AAuraPlayerState::SetXP(const int32 InXP)
{
	XP = InXP;
	XP = FMath::Max(XP, 0);
	OnXPChanged.Broadcast(XP);
}

void AAuraPlayerState::SetAttributePoints(const int32 InAttributePoints)
{
	AttributePoints = InAttributePoints;
	AttributePoints = FMath::Max(AttributePoints, 0);
	OnAttributePointsChanged.Broadcast(AttributePoints);
}

void AAuraPlayerState::SetSpellPoints(const int32 InSpellPoints)
{
	SpellPoints = InSpellPoints;
	SpellPoints = FMath::Max(SpellPoints, 0);
	OnSpellPointsChanged.Broadcast(SpellPoints);
}

void AAuraPlayerState::AddToLevel(const int32 InLevel)
{
	SetLevel(Level + InLevel);
}

void AAuraPlayerState::AddToXP(const int32 InXP)
{
	SetXP(XP + InXP);
}


void AAuraPlayerState::AddToAttributePoints(const int32 InAttributePoints)
{
	SetAttributePoints(AttributePoints + InAttributePoints);
}

void AAuraPlayerState::AddToSpellPoints(const int32 InSpellPoints)
{
	SetSpellPoints(SpellPoints + InSpellPoints);
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel) const
{
	OnLevelChanged.Broadcast(Level);
}

void AAuraPlayerState::OnRep_XP(int32 OldXP) const
{
	OnXPChanged.Broadcast(XP);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldAttributePoints) const
{
	OnAttributePointsChanged.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldSpellPoints) const
{
	OnSpellPointsChanged.Broadcast(SpellPoints);
}

