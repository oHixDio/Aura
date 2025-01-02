// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraPlayer.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraPlayer::AAuraPlayer()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 400.f, 0);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AAuraPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// ActorInfoをサーバーで設定.
	InitAbilityActorInfo();
	
}

void AAuraPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// ActorInfoをクライアントで設定.
	InitAbilityActorInfo();
}

void AAuraPlayer::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();

	// LocalでOverlayWidgetを表示する.
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (AAuraHUD* HUD = PlayerController->GetHUD<AAuraHUD>())
		{
			HUD->InitOverlay(PlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}