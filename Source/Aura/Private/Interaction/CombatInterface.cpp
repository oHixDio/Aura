// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/CombatInterface.h"

// Add default functionality here for any ICombatInterface functions that are not pure virtual.
float ICombatInterface::GetPlayerLevel() const
{
	return 0.f;
}

FVector ICombatInterface::GetCombatSocketLocation() const
{
	return FVector::ZeroVector;
}

