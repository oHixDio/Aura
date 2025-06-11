// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraPlayer.h"

#include "AuraGameplayTags.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "Player/Data/LevelUpInfo.h"
#include "UI/HUD/AuraHUD.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

AAuraPlayer::AAuraPlayer()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;
	
	LevelUpNiagaraSystemComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraSystemComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraSystemComponent->bAutoActivate = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 400.f, 0);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CharacterClass = ECharacterClass::Elementalist;
}

void AAuraPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// ActorInfoをサーバーで設定.
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

void AAuraPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// ActorInfoをクライアントで設定.
	InitAbilityActorInfo();
	
}

void AAuraPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraPlayer::InitializeDefaultAttributes() const
{
	Super::InitializeDefaultAttributes();

	ApplyEffectToSelf(DefaultPrimaryAttributesClass, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributesClass, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributesClass, 1.f);
}

float AAuraPlayer::GetPlayerLevel_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraPlayer::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
	ClientShowLevelUpMessage();
	MulticastLevelUpSound();
}

int32 AAuraPlayer::GetPlayerXP_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetXP();
}

int32 AAuraPlayer::GetAttributePointReward_Implementation(const int32 Level) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	check(AuraPlayerState->LevelUpInfo);
	check(AuraPlayerState->LevelUpInfo->LevelUpInformation.IsValidIndex(Level));
	return AuraPlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointReward;
}

int32 AAuraPlayer::GetSpellPointReward_Implementation(const int32 Level) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	check(AuraPlayerState->LevelUpInfo);
	check(AuraPlayerState->LevelUpInfo->LevelUpInformation.IsValidIndex(Level))
	return AuraPlayerState->LevelUpInfo->LevelUpInformation[Level].SpellPointReward;
}

int32 AAuraPlayer::FindLevelForXP_Implementation(const int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	check(AuraPlayerState->LevelUpInfo);
	return AuraPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

void AAuraPlayer::AddToXP_Implementation(const int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToXP(InXP);
}

void AAuraPlayer::AddToPlayerLevel_Implementation(const int32 InLevel)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToLevel(InLevel);
}

void AAuraPlayer::AddToAttributePointReward_Implementation(const int32 InPoint)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToAttributePoints(InPoint);
}

void AAuraPlayer::AddToSpellPointReward_Implementation(const int32 InPoint)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToSpellPoints(InPoint);
}

int32 AAuraPlayer::GetAttributePoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetAttributePoints();
}

void AAuraPlayer::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);	// InitAbilityActorInfoを呼び出したら.
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();			// この関数も必ず呼び出す.
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

	if (bTakeHitReact) AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Events_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraCharacterBase::HitReactTagChanged);

	InitializeDefaultAttributes();
}

void AAuraPlayer::MulticastLevelUpParticles_Implementation()
{
	if (IsValid(LevelUpNiagaraSystemComponent))
	{
		const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
		const FVector LevelUpNiagaraLocation = LevelUpNiagaraSystemComponent->GetComponentLocation();
		const FRotator LookUpRotation = (CameraLocation - LevelUpNiagaraLocation).Rotation();
		LevelUpNiagaraSystemComponent->SetWorldRotation(LookUpRotation);
		LevelUpNiagaraSystemComponent->Activate(true);
	}
}

void AAuraPlayer::MulticastLevelUpSound_Implementation()
{
	if (IsValid(LevelUpSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, LevelUpSound, GetActorLocation());
	}
}

void AAuraPlayer::ClientShowLevelUpMessage_Implementation()
{
	if (FloatingLevelUpWidgetClass && IsLocallyControlled())
	{
		UWidgetComponent* FloatingDamage = NewObject<UWidgetComponent>(this, FloatingLevelUpWidgetClass);
		FloatingDamage->RegisterComponent();
		FloatingDamage->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		FloatingDamage->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
}
