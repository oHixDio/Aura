// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	// AttributeSetからVigorを取得.
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	// 適用先キャラの属性値によって変動する為Target指定.
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	// 実行時に動的評価
	VigorDef.bSnapshot = false;

	// Vigorを計算中に使用可能にする.
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// 属性の評価に必要なタグを取得.
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// Vigorの値を取得する. マイナスの場合、0に調整.
	float Magnitude{};
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluateParameters, Magnitude);
	const float Vigor = FMath::Max(Magnitude, 0.f);

	// CombatInterfaceを実装している前提に、Levelを取得.
	TScriptInterface<ICombatInterface> CombatInterface = Spec.GetContext().GetSourceObject();
	const float Level = CombatInterface->GetPlayerLevel();

	// 計算して返却.
	return 100.f + 2.f * Vigor + 10.f * Level;
}
