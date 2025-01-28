// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraAssetManager.h"

#include "AuraGameplayTags.h"

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
}
