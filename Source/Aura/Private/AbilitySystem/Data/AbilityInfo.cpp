// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AbilityInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityForTag(const FGameplayTag& Tag, bool bLogNotFound)
{
	for (const FAuraAbilityInfo& AbilityInfo : AbilityInformation)
	{
		if (AbilityInfo.AbilityTag.MatchesTag(Tag))
		{
			return AbilityInfo;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Can't find info for AttributeTag [%s] on AttributeInfo [%s]"), *Tag.ToString(), *GetNameSafe(this));
	}

	return FAuraAbilityInfo();
}
