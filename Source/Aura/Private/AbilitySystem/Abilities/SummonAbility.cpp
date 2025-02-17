// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/SummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> USummonAbility::GetSpawnLocations()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread, FVector::UpVector);
	const float DeltaSpread = (NumOfMinions > 1) ? (2 * SpawnSpread) / (NumOfMinions - 1) : 0.f;
	TArray<FVector> SpawnLocations;
	
	for (int32 i = 0; i < NumOfMinions; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		FVector SpawnLocation = Location + Direction * FMath::FRandRange(SpawnMinDistance, SpawnMaxDistance);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, SpawnLocation + FVector::UpVector * 400.f, SpawnLocation + FVector::UpVector * -400.f, ECollisionChannel::ECC_Visibility);

		if (Hit.bBlockingHit)
		{
			SpawnLocation = Hit.ImpactPoint;
		}
		else
		{
			continue;
		}

		SpawnLocations.Add(SpawnLocation);
	}
	
	return SpawnLocations;
}

TSubclassOf<APawn> USummonAbility::GetRandomSpawnMinion() const
{
	const int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
	check(MinionClasses[Selection]);
	return MinionClasses[Selection];
}
