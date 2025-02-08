// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraAssetManager.h"

#include "AuraGameplayTags.h"
#include "AbilitySystemGlobals.h"

const UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);
	const UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(&UAssetManager::Get());
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FAuraGameplayTags::InitializeNativeGameplayTags();

	// Target Dataを扱う時に必須！　(UE5.3以降は内部で実行してくれている為、記述不要。)
	// UAbilitySystemGlobals::Get().InitGlobalData();
}
