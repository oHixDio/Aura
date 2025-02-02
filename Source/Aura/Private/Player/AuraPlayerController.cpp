// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/AuraInputComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// IMCの設定.
	/**
	 * LocalPlayerはローカルデバイス上に存在しているため、
	 * サーバー上でクライアントのPlayerControllerインスタンス生成時にこのラインを通過した際、
	 * LocalPlayerはnullptrとなる。
	 * そのため、assertではなく、ifチェックでnullptrチェックを行う.
	 */
	check(AuraContext);
	if (UEnhancedInputLocalPlayerSubsystem* EnhancedSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		EnhancedSubsystem->AddMappingContext(AuraContext, 0);
	}

	/**
	 * カーソル設定!!
	 * 表示モード.
	 * Window外に出られる.
	 * GameとUIに干渉できる.
	 */
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAbilityAction(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	// GEngine->AddOnScreenDebugMessage(1, 5, FColor::Green, *InputTag.ToString());
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() == nullptr) return;
	GetASC()->AbilityInputTagReleased(InputTag);
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC() == nullptr) return;
	GetASC()->AbilityInputTagHeld(InputTag);
}

void AAuraPlayerController::CursorTrace()
{
	// カーソルがHoverしているActorを取得できる便利関数.
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastHoverActor = ThisHoverActor;
	ThisHoverActor = CursorHit.GetActor();
	
	/**
	 * CursorHit結果のシナリオ.
	 *	A. どちらもnull
	 *		- なにもしない.
	 *	B. ThisHoverActorだけValid
	 *		- ThisHoverActorをハイライト
	 *	C. LastHoverActorだけValid
	 *		- LastHoverActorをアンハイライト
	 *	D. どちらもValid　かつ　違うActor
	 *		- ThisHoverActorをハイライト
	 *		  LastHoverActorをアンハイライト
	 *	E. どちらもValid　かつ　同じActor
	 *		- 何もしない.
	 */
	
	if (LastHoverActor == nullptr && ThisHoverActor == nullptr)
	{/* A */
	}
	else if (LastHoverActor == nullptr && ThisHoverActor)
	{/* B */
		ThisHoverActor->Highlight();
	}
	else if (LastHoverActor && ThisHoverActor == nullptr)
	{/* C */
		LastHoverActor->UnHighlight();
	}
	else if (LastHoverActor && ThisHoverActor && LastHoverActor != ThisHoverActor)
	{/* D */
		LastHoverActor->UnHighlight();
		ThisHoverActor->Highlight();
	}
	else if (LastHoverActor && ThisHoverActor && LastHoverActor == ThisHoverActor)
	{/* E */
	}
}
