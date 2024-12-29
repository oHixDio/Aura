// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Highlightable.generated.h"

UINTERFACE(MinimalAPI)
class UHighlightable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IHighlightable
{
	GENERATED_BODY()

public:
	virtual void Highlight() = 0;
	virtual void UnHighlight() = 0;
};
