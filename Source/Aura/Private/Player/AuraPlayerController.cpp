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

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
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

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	// 左マウスボタン押下時の処理.
	if (InputTag.IsValid() && InputTag.MatchesTagExact(FAuraGameplayTags::Get().Input_LMB))
	{
		bTargeting = ThisHoverActor ? true : false;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	bool bByLMB = InputTag.IsValid() && InputTag.MatchesTagExact(FAuraGameplayTags::Get().Input_LMB);
	if (!bByLMB || bTargeting)
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}

	// 移動処理！	
	if (!bTargeting)
	{
		APawn* ControlledPawn = GetPawn<APawn>();
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
					DrawDebugSphere(GetWorld(),PathLoc, 8.f, 8, FColor::Orange, false, 5.f);
				}
				bAutoRunning = true;	// AutoRun状態に.
			}
		}
		// Hold時の情報クリア.
		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	bool bByLMB = InputTag.IsValid() && InputTag.MatchesTagExact(FAuraGameplayTags::Get().Input_LMB);
	if (!bByLMB || bTargeting)
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}

	// Target対象をホバーしていない.Moveするぞ！
	if (!bTargeting)
	{
		// Hold Timeを測る.
		FollowTime += GetWorld()->GetDeltaSeconds();

		// マウスの座標取得.
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);
		if (Hit.bBlockingHit)
		{
			CashedDestination = Hit.ImpactPoint;
		}

		// 移動.
		if (APawn* ControlledPawn = GetPawn<APawn>())
		{
			FVector WorldDirection = (CashedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);	// Rep対応.
		}
	}
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
