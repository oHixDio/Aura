// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacterBase.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/AuraAbilitySystemFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	GetCharacterMovement()->RotationRate = FRotator(0, BaseRotationSpeedYaw, 0);

	if (HasAuthority())
	{
		// Common AbilityのGiveを実行.（HitReactとか）
		UAuraAbilitySystemFunctionLibrary::GiveDefaultAbilities(this,AbilitySystemComponent, CharacterClass);
	}
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const
{
	const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
	if (AuraTags.Events_Montage_Weapon == MontageTag && WeaponMesh)
	{
		return WeaponMesh->GetSocketLocation(CombatSocketName::WeaponSocket);
	}
	if (AuraTags.Events_Montage_WeaponTip == MontageTag && WeaponMesh)
	{
		return WeaponMesh->GetSocketLocation(CombatSocketName::WeaponTipSocket);
	}
	if (AuraTags.Events_Montage_LeftHand == MontageTag && GetMesh())
	{
		return GetMesh()->GetSocketLocation(CombatSocketName::LeftHandSocket);
	}
	if (AuraTags.Events_Montage_RightHand == MontageTag && GetMesh())
	{
		return GetMesh()->GetSocketLocation(CombatSocketName::RightHandSocket);
	}
	return FVector::ZeroVector;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation() const
{
	return HitReactMontage;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation() const
{
	return TaggedMontages;
}

void AAuraCharacterBase::Die()
{
	WeaponMesh->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastDie();
}

void AAuraCharacterBase::MulticastDie_Implementation()
{
	// Weaponをラグドール化.
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	// Meshをラグドール化.
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	// カプセルコリジョンをなくす.
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	Dissolve();
	bIsDead = true;
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return bIsDead;
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& EffectClass, const float Level) const
{
	check(GetAbilitySystemComponent());
	check(EffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(EffectClass,Level,ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributesClass, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributesClass, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributesClass, 1.f);
}

void AAuraCharacterBase::AddCharacterAbilities() const
{
	if (!HasAuthority()) return;
	
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);

	AuraASC->AddCharacterAbilities(StartupAbilities);
}

void AAuraCharacterBase::Dissolve()
{
	if (MeshDissolveMaterial)
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(MeshDissolveMaterial, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);
		StartMeshDissolveTimeLine(DynamicMatInst);
	}
	if (WeaponDissolveMaterial)
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterial, this);
		WeaponMesh->SetMaterial(0, DynamicMatInst);
		StartWeaponDissolveTimeLine(DynamicMatInst);
	}
}

void AAuraCharacterBase::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	GetCharacterMovement()->RotationRate = bHitReacting ? FRotator::ZeroRotator : FRotator(0, BaseRotationSpeedYaw, 0);
}
