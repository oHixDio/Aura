// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/CombatInterface.h"

namespace CombatSocketName
{
	const FName WeaponSocket = FName("WeaponSocket");
	const FName WeaponTipSocket = FName("TipSocket");
	const FName LeftHandSocket = FName("LeftHandSocket");
	const FName RightHandSocket = FName("RightHandSocket");
}

// Add default functionality here for any ICombatInterface functions that are not pure virtual.
float ICombatInterface::GetPlayerLevel() const
{
	return 0.f;
}

