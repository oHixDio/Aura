// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
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
		// SpawnRotation.Pitch = 0.f;
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
		if (const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo()))
		{
			FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
			EffectContextHandle.SetAbility(this);
			EffectContextHandle.AddSourceObject(AuraProjectile);
			TArray<TWeakObjectPtr<AActor>> Actors;
			Actors.Add(AuraProjectile);
			EffectContextHandle.AddActors(Actors);
			FHitResult HitResult;
			HitResult.Location = ProjectileTargetLocation;
			EffectContextHandle.AddHitResult(HitResult);
			
			const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffect, GetAbilityLevel(), EffectContextHandle);
			
			// SetByCallerとしてダメージを伝える。各ダメージはタグがマッピングされている。
			for (const TTuple<FGameplayTag, FScalableFloat>& DamageType : DamageTypes)
			{
				UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType.Key, DamageType.Value.GetValueAtLevel(GetAbilityLevel()));
			}
			
			AuraProjectile->DamageEffectSpecHandle = SpecHandle;
		}	
		
		AuraProjectile->FinishSpawning(SpawnTransform);
	}
}
