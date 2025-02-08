// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AuraAssetManager.generated.h"

/**
 * Singleton クラスです.
 * AuraGameplayTags を初期化しています。
 */
UCLASS()
class AURA_API UAuraAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static const UAuraAssetManager& Get();

	/** 最初期にアセットを初期化するのに最適な場所. */
	virtual void StartInitialLoading() override;
};
