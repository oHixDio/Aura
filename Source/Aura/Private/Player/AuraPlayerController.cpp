// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

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

	check(AuraContext);
	UEnhancedInputLocalPlayerSubsystem* EnhancedSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(EnhancedSubsystem);
	EnhancedSubsystem->AddMappingContext(AuraContext, 0);

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

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
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

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastHoverActor = ThisHoverActor;
	ThisHoverActor = CursorHit.GetActor();
	
	/**
	 * CursorHit結果のシナリオ.
	 *	A. LastHoverActor null & ThisHoverActor null
	 *		- なにもしない.
	 *	B. LastHoverActor null & ThisHoverActor valid
	 *		- Highlight ThisHoverActor
	 *	C. LastHoverActor valid & ThisHoverActor null
	 *		- UnHighlight LastHoverActor
	 *	D. LastHoverActor valid & ThisHoverActor valid & LastHoverActor != ThisHoverActor
	 *		- Highlight ThisHoverActor
	 *		  UnHighlight LastHoverActor
	 *	E. LastHoverActor valid & ThisHoverActor valid & LastHoverActor == ThisHoverActor
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
