// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AuraInputConfig.h"


const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag Tag, bool bLogNotFound) const
{
	for (const FAuraInputAction& Action : AbilityInputActions)
	{
		if (Action.InputTag.MatchesTagExact(Tag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find Tag '%s' On InputAction '%s'"), *Tag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
