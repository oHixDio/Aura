// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxMana.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	// AttributeSetからIntelligenceを取得.
	IntelligenceDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute();
	// 適用先キャラの属性値によって変動する為Target指定.
	IntelligenceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	// 実行時に動的評価
	IntelligenceDef.bSnapshot = false;

	// Intelligenceを計算中に使用可能にする.
	RelevantAttributesToCapture.Add(IntelligenceDef);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// 属性の評価に必要なタグを取得.
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// Intelligenceの値を取得する. マイナスの場合、0に調整.
	float Magnitude{};
	GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluateParameters, Magnitude);
	const float Intelligence = FMath::Max(Magnitude, 0.f);

	// CombatInterfaceを実装している前提に、Levelを取得.
	TScriptInterface<ICombatInterface> CombatInterface = Spec.GetContext().GetSourceObject();
	const float Level = CombatInterface->GetPlayerLevel();

	// 計算して返却.
	return 80.f + 2.f * Intelligence + 10.f * Level;
}