// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	// Avatar => Pawn.
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	
	check(ProjectileClass);

	if (TScriptInterface<ICombatInterface> CombatActor = GetAvatarActorFromActorInfo())
	{
		const FVector SpawnLocation = CombatActor->GetCombatSocketLocation();
		FRotator SpawnRotation = (ProjectileTargetLocation - SpawnLocation).Rotation();
		// 地面と水平に飛ばす.
		SpawnRotation.Pitch = 0.f;
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnLocation);
		SpawnTransform.SetRotation(SpawnRotation.Quaternion());

		AAuraProjectile* AuraProjectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

		// DamageEffectを作成してProjectileに渡す.
		if (UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo()))
		{
			AuraProjectile->DamageEffectSpecHandle= SourceASC->MakeOutgoingSpec(DamageEffect, GetAbilityLevel(), SourceASC->MakeEffectContext());
		}	
		
		AuraProjectile->FinishSpawning(SpawnTransform);
	}
}
