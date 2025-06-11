// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AbilityDescriptionInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAbilityDescriptionInfo UAbilityDescriptionInfo::FindAbilityDescriptionForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	// TODO: ここにInfoを取得するコードを記述する。
	// このInfoはSpellGlobeが更新される条件
		// セレクトされたとき
		// Abilityのステータスが更新されたとき。

	for (const FAuraAbilityDescriptionInfo& DescriptionInfo : AbilityDescriptionInformation)
	{
		if (DescriptionInfo.AbilityTag.MatchesTag(AbilityTag))
		{
			return DescriptionInfo;
		}
	}
	
	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Can't find Description for AbilityTag [%s] on AbilityDescriptionInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
	}

	return FAuraAbilityDescriptionInfo();
}
