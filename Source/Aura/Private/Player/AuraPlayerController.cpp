// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "AuraGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "UI/Widget/FloatingDamageComponent.h"
#include "GameFramework/Character.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	AutoRun();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	/*
	 *	InputMappingContextの設定！！
	 */
	check(AuraContext);
	if (UEnhancedInputLocalPlayerSubsystem* EnhancedSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		EnhancedSubsystem->AddMappingContext(AuraContext, 0);
	}

	/*
	 * カーソルの設定！！
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

	/*
	 *	InputAction 設定！
	 */
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction,ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction,ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	AuraInputComponent->BindAbilityAction(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	// UAuraAbilitySystemComponentにキャストして返却.
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	// 既にキャスト済みならそのまま返却.
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

void AAuraPlayerController::ClickToMoveByPressed()
{
	bTargeting = ThisHoverActor ? true : false;
	bAutoRunning = false;
}

void AAuraPlayerController::ClickToMoveByReleased()
{
	const APawn* ControlledPawn = GetPawn<APawn>();
	// Press時間が閾値以下ならClickToMove！
	if (ControlledPawn && FollowTime <= ShortPressThreshold)
	{
		// Pawnの現在座標から、押下時のマウス座標までのPathを取得する.
		if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CashedDestination))
		{
			// Splineに新しいPathPointsを代入.
			Spline->ClearSplinePoints();
			for (FVector& PathLoc : NavPath->PathPoints)
			{
				Spline->AddSplinePoint(PathLoc, ESplineCoordinateSpace::World);
			}
			if (!NavPath->PathPoints.IsEmpty()) CashedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
			bAutoRunning = true;	// AutoRun状態に.
		}
	}
	// Hold時の情報クリア.
	FollowTime = 0.f;
	bTargeting = false;
}

void AAuraPlayerController::ClickToMoveByHeld()
{
	// Hold Timeを測る.
	FollowTime += GetWorld()->GetDeltaSeconds();

	// マウスの座標取得.
	if (CursorHit.bBlockingHit) CashedDestination = CursorHit.ImpactPoint;

	// 移動.
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		FVector WorldDirection = (CashedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection);	// Rep対応.
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.IsValid() && InputTag.MatchesTagExact(FAuraGameplayTags::Get().Input_LMB))
	{
		ClickToMoveByPressed();
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	const bool bByLMB = InputTag.IsValid() && InputTag.MatchesTagExact(FAuraGameplayTags::Get().Input_LMB);
	if (!bByLMB || bTargeting || bShiftPressed)
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}

	ClickToMoveByReleased();
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	const bool bByLMB = InputTag.IsValid() && InputTag.MatchesTagExact(FAuraGameplayTags::Get().Input_LMB);
	if (!bByLMB || bTargeting || bShiftPressed)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}

	ClickToMoveByHeld();
}

void AAuraPlayerController::CursorTrace()
{
	// カーソルがHoverしているActorを取得できる便利関数.
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastHoverActor = ThisHoverActor;
	ThisHoverActor = CursorHit.GetActor();
	
	if (LastHoverActor != ThisHoverActor)
	{
		if (LastHoverActor) LastHoverActor->UnHighlight();
		if (ThisHoverActor) ThisHoverActor->Highlight();
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		// 現在座標からSpline座標の中で一番近いLocation取得.
		FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		// SplineLocationまでの移動を取得.
		FVector WorldDirection = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(WorldDirection);

		// Mouse押下Locationまでの距離がAutoRun許可範囲を下回ったら、AutoRunを止める.
		const float DistanceToDestination = (LocationOnSpline - CashedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::ClientShowFloatingDamage_Implementation(const float DamageValue, ACharacter* Target)
{
	if (Target && FloatingDamageWidgetClass)
	{
		UFloatingDamageComponent* FloatingDamage = NewObject<UFloatingDamageComponent>(Target, FloatingDamageWidgetClass);
		FloatingDamage->RegisterComponent();
		FloatingDamage->AttachToComponent(Target->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		FloatingDamage->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		FloatingDamage->SetDamageValue(DamageValue);
		
	}
}
