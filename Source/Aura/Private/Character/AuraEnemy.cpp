// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraEnemy.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemFunctionLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "UI/WidgetController/EnemyWidgetController.h"
#include "Components/CapsuleComponent.h"
#include "UI/Widget/AuraUserWidget.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCustomDepthStencilValue(250);
	WeaponMesh->SetCustomDepthStencilValue(250);

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>("HealthBarWidget");
	HealthBarWidget->SetupAttachment(GetCapsuleComponent());
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetDrawAtDesiredSize(true);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAuraEnemy::Highlight()
{
	bHighlighted = true;
	GetMesh()->SetRenderCustomDepth(true);
	WeaponMesh->SetRenderCustomDepth(true);
}

void AAuraEnemy::UnHighlight()
{
	bHighlighted = false;
	GetMesh()->SetRenderCustomDepth(false);
	WeaponMesh->SetRenderCustomDepth(false);
}

float AAuraEnemy::GetPlayerLevel() const
{
	return Level;
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();
}

void AAuraEnemy::InitAbilityActorInfo()
{
	// EnemyはOwnerとAvatarはどちらも自分自身(Character).
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	InitializeDefaultAttributes();
	InitEnemyWidget();
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemFunctionLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

UEnemyWidgetController* AAuraEnemy::GetEnemyWidgetController()
{
	if (EnemyWidgetController == nullptr)
	{
		check(EnemyWidgetControllerClass);
		EnemyWidgetController = NewObject<UEnemyWidgetController>(this, EnemyWidgetControllerClass);
		const FWidgetControllerParams WidgetControllerParams(nullptr, nullptr, AbilitySystemComponent, AttributeSet);
		EnemyWidgetController->SetWidgetControllerParams(WidgetControllerParams);
		EnemyWidgetController->BindCallbacksToDependencies();
	}

	return EnemyWidgetController;
}

void AAuraEnemy::InitEnemyWidget()
{
	check(HealthBarWidgetClass)
	UAuraUserWidget* HealthBar = CreateWidget<UAuraUserWidget>(GetWorld(), HealthBarWidgetClass);
	HealthBar->SetWidgetController(GetEnemyWidgetController());
	HealthBarWidget->SetWidget(HealthBar);
	EnemyWidgetController->BroadcastInitialValues();
}
