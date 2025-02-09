// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemFunctionLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

// C++のみで扱う生ストラクトとして定義.
struct AuraDamageStatics
{
	// 属性をキャプチャするための定義.
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagToCaptureDefs;
	
	AuraDamageStatics()
	{
		// UAuraAttributeSet の 属性を Target(攻撃を受ける側) から 
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);

		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
		TagToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, BlockChanceDef);
		TagToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, ArmorDef);
		TagToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
		TagToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);
		TagToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);

		TagToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, FireResistanceDef);
		TagToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, LightningResistanceDef);
		TagToCaptureDefs.Add(Tags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
		TagToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, PhysicalResistanceDef);
	}
};

// AuraDamageStaticsのシングルトンを取得する関数.
static const AuraDamageStatics& DamageStatics()
{
	// プログラムに全体で１つのインスタンスとなる.
	static AuraDamageStatics DamageStatics;
	return DamageStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	// このカスタム計算クラスが 属性をキャプチャするように設定.
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);

	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
	
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// ~必要なデータを取得.
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const TScriptInterface<ICombatInterface> SourceCombatActor = SourceAvatar;
	const TScriptInterface<ICombatInterface> TargetCombatActor = TargetAvatar;
		
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemFunctionLibrary::GetCharacterClassInfo(SourceAvatar);
	if (!IsValid(CharacterClassInfo)) return;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	

	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	// ~
	

	// SetByCallerからDamageを取得.
	float Damage = 0.f;

	for (const TTuple<FGameplayTag, FGameplayTag>& Pair  : FAuraGameplayTags::Get().DamageTypeToResistances)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;
		
		checkf(AuraDamageStatics().TagToCaptureDefs.Contains(ResistanceTag), TEXT("TagToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceTag.ToString());
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = AuraDamageStatics().TagToCaptureDefs[ResistanceTag];
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key, false);
		
		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParams, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);
		DamageTypeValue *= ( 100.f - Resistance ) / 100.f;
		
		Damage += DamageTypeValue;
	}
	
	// ~ Attributeを取得.
	float TargetBlockChance{};
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef,EvaluateParams,TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(0.f, TargetBlockChance);

	float TargetArmor{};
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef,EvaluateParams,TargetArmor);
	TargetArmor = FMath::Max<float>(0.f, TargetArmor);

	float SourceArmorPenetration{};
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef,EvaluateParams,SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0.f, SourceArmorPenetration);

	float SourceCriticalHitChance{};
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef,EvaluateParams,SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(0.f, SourceCriticalHitChance);

	float SourceCriticalHitDamage{};
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef,EvaluateParams,SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(0.f, SourceCriticalHitDamage);

	float TargetCriticalHitResistance{};
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef,EvaluateParams,TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(0.f, TargetCriticalHitResistance);
	// ~

	// ~ Damage計算.

	// Criticalかどうかを判定.
	const bool bCriticalHit = FMath::RandRange(0, 100) < SourceCriticalHitChance;

	// 有効クリティカルヒットダメージの係数を取得.
	FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficientCurveTable->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(SourceCombatActor->GetPlayerLevel());

	// 実際に有効なクリティカルダメージを算出.
	const float EffectiveCriticalHitDamage = SourceCriticalHitDamage * (100 - TargetCriticalHitResistance * CriticalHitResistanceCoefficient) / 100.f;
	
	// クリティカルヒットダメージの係数を取得.
	FRealCurve* CriticalHitDamageCurve = CharacterClassInfo->DamageCalculationCoefficientCurveTable->FindCurve(FName("CriticalHitDamage"), FString());
	const float CriticalHitDamageCoefficient = CriticalHitDamageCurve->Eval(SourceCombatActor->GetPlayerLevel());

	// CriticalHitDamageを算出.
	const float RealCriticalHitDamage = Damage * 2.f + EffectiveCriticalHitDamage * CriticalHitDamageCoefficient;

	// Damageにクリティカルダメージを適用
	Damage = bCriticalHit ? RealCriticalHitDamage : Damage;
	
	// Damageを確立でBlockする.
	const bool bBlocked = FMath::RandRange(0, 100) < TargetBlockChance;

	// ブロックされた場合ダメージ半減.
	Damage = bBlocked ? Damage / 2.f : Damage;
	
	// ArmorPenetrationの係数を取得.(Sourceから)
	FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficientCurveTable->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatActor->GetPlayerLevel());
	
	// Armor浸食を計算して、実際に効果あるArmor値を算出.
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

	// EffectiveArmorの係数を取得.(Targetから)
	FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficientCurveTable->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatActor->GetPlayerLevel());
	
	// Damageに適用するArmor値を算出.
	const float RealArmor = (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;

	// DamageをTargetArmorで防ぐ.
	Damage *= RealArmor;
	
	// ~

	// DamageTypeを保存する.
	UAuraAbilitySystemFunctionLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	UAuraAbilitySystemFunctionLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

	// 計算結果を出力データに追加 (Armor の変更を適用). 変更するAttr, 加算方式で適用, 計算後の Armor 値.
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(),EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
