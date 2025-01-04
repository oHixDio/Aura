// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/EffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AEffectActor::AEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AEffectActor::ApplyEffectToTarget(AActor* TargetActor, const TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;	// ← ASCを持っていないActorと接触する可能性がある.

	check(GameplayEffectClass);	// ← GEClassは設定していなくてはならない.
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);	// 生成元となったオブジェクト(今回はEffectActor)を設定.
	// GameplayEffectの仕様となるオブジェクトのラッパ.単にGameplayEffectを使用するより最適化されている.
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, EffectContextHandle);
	// TargetにGameplayEffectを適用する.
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());	// ← ラッパからSpecの元データを取得している.

	// InfiniteEffectをRemoveするには保持しなくてはならない.
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteRemovalPolicy ==EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		// Effect毎に複数のActorがそのEffectを実行している可能性があるため、Mapを使用する.
		ActiveEffectHandles.Add(ActiveGameplayEffectHandle, TargetASC);
	}
}

void AEffectActor::OnBeginOverlap(AActor* TargetActor)
{
	// BeginOverlapで実行する設定であるGameplayEffectを実行する.
	
	if (InstantApplicationPolicy == EEffectApplicationPolicy::ApplyOnBeginOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationApplicationPolicy == EEffectApplicationPolicy::ApplyOnBeginOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteApplicationPolicy == EEffectApplicationPolicy::ApplyOnBeginOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AEffectActor::OnEndOverlap(AActor* TargetActor)
{
	// EndOverlapで実行する設定であるGameplayEffectを実行する.
	
	if (InstantApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);	// ← RemovalPolicyがEndOverlapの場合、ここで実行されてもすぐに解除される.
	}

	// Infinite限定の処理.EndOverlapでGameplayEffectが解除される.
	if (InfiniteRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;
		
		TArray<FActiveGameplayEffectHandle> RemovedEffects;

		// 先ずは解除処理.
		for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandle : ActiveEffectHandles)
		{
			// TargetActor
			if (ActiveEffectHandle.Value == TargetASC)
			{
				TargetASC->RemoveActiveGameplayEffect(ActiveEffectHandle.Key, 1);	// ← 複数スタックされていても、１つだけ解除する.
				RemovedEffects.Add(ActiveEffectHandle.Key); // ← ここでMapから削除しない理由はforループ中だから.
			}
		}

		// 解除したEffectはMapから削除する.
		for (FActiveGameplayEffectHandle& RemovedEffect : RemovedEffects)
		{
			ActiveEffectHandles.FindAndRemoveChecked(RemovedEffect);
		}
	}
}
