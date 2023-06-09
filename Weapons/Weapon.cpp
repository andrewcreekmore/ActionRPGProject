// © 2022 Andrew Creekmore 


#include "../Weapons/Weapon.h"
#include "../ActionRPGProject.h"
#include "../Components/InventoryComponent.h"
#include "../Character/Main.h"
#include "../Character/MainPlayerController.h"
#include "../DebugMacros.h"
#include "../Enemies/Enemy.h"
#include "../Items/EquippableItem.h"
#include "../Items/WeaponItem.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveVector.h"
#include "DrawDebugHelpers.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"


// sets default values
AWeapon::AWeapon()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = WeaponMesh;

	WeaponType = EWeaponType::EMS_MAX;

	bPlayingAttackAnim = false;
	bPlayingLightAttackAnim = false;
	bPlayingRunningLightAttackAnim = false;
	bPlayingHeavyAttackAnim = false;
	bPlayingRunningHeavyAttackAnim = false;
	bIsEquipped = false;
	bWantsToAttack = false;
	bPendingEquip = false;
	bPendingUnequip = false;
	CurrentState = EWeaponState::Idle;
	AttachSocketStored = FName("Weapon_Stored_Socket");
	AttachSocketDrawn = FName("WEAPON_R");
	LastAttackTime = 0.0f;
	StoreWeaponOutOfCombatDelayAmount = 30.0f;

	LightAttackAnimationCounter = 0;
	HeavyAttackAnimationCounter = 0;

	LightSoundSelector = 0;
	HeavySoundSelector = 0;
	EquipSoundVolumeMultiplier = 1.f;
	UnequipSoundVolumeMultiplier = 1.f;
	
	bIsCollisionActive = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

}


// called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	PawnOwner = Cast<AMain>(GetOwner());
}


void AWeapon::OnEquip()
{
	AttachMeshToPawn();

	bPendingEquip = true;
	DetermineWeaponState();

	OnEquipFinished();

	if (PawnOwner)
	{
		if (Item->bShouldNotifyOnInventoryAdd)
		{
			if (EquipSound)
			{ UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation(), EquipSoundVolumeMultiplier); }
		}
	}
}


void AWeapon::OnEquipFinished()
{
	AttachMeshToPawn();
	
	bIsEquipped = true;
	bPendingEquip = false;

	PawnOwner->bHasWeaponEquipped = true;
	PawnOwner->bIsEquipping = false;

	DetermineWeaponState();
}


void AWeapon::OnUnequip()
{
	if (UnequipSound && !PawnOwner->bIsEquipping) // don't play unequip sound if another one is being equipped in its place 
	{ UGameplayStatics::PlaySoundAtLocation(this, UnequipSound, GetActorLocation(), UnequipSoundVolumeMultiplier); }

	bIsEquipped = false;
	PawnOwner->bHasWeaponEquipped = false;
	StopAttack();

	if (bPendingEquip)
	{
		StopWeaponAnimation(EquipAnim);
		bPendingEquip = false;
		GetWorldTimerManager().ClearTimer(TimerHandle_OnEquipFinished);
	}

	DetermineWeaponState();
}


bool AWeapon::IsEquipped() const
{
	return bIsEquipped;
}


bool AWeapon::IsAttachedToPawn() const
{
	return bIsEquipped || bPendingEquip;
}


void AWeapon::StartAttack()
{
	if (!bWantsToAttack)
	{
		bWantsToAttack = true;
		DetermineWeaponState();
	}
}


void AWeapon::StopAttack()
{
	if (bWantsToAttack)
	{
		bWantsToAttack = false;
		DetermineWeaponState();
	}
}


bool AWeapon::CanAttack() const
{
	bool bCanAttack = PawnOwner != nullptr;
	bool bStateOKToAttack = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Attacking));
	return ((bCanAttack == true) && (bStateOKToAttack == true));
}


EWeaponState AWeapon::GetCurrentState() const
{
	return CurrentState;
}


USkeletalMeshComponent* AWeapon::GetWeaponMesh()
{
	return WeaponMesh;
}


class AMain* AWeapon::GetPawnOwner() const
{
	return PawnOwner;
}


float AWeapon::GetEquipStartedTime() const
{
	return EquipStartedTime;
}


float AWeapon::GetEquipDuration() const
{
	return EquipDuration;
}


void AWeapon::DrawWeapon()
{
	PlayWeaponAnimation(DrawAnim);
	PawnOwner->ShowHUDCall();

	ResetStoreWeaponTimer();
}


void AWeapon::StoreWeapon()
{
	if (!PawnOwner->bLockedOn && !PawnOwner->bBlocking && PawnOwner->bHasWeaponDrawn)
	{	
		PlayWeaponAnimation(StoreAnim);
		PawnOwner->HideHUDCall();
		GetWorldTimerManager().ClearTimer(StoreWeaponOutOfCombatTimer);
	}

	else
	{ ResetStoreWeaponTimer(); }
}


void AWeapon::ResetStoreWeaponTimer()
{
	GetWorldTimerManager().ClearTimer(StoreWeaponOutOfCombatTimer);
	GetWorldTimerManager().SetTimer(StoreWeaponOutOfCombatTimer, this, &AWeapon::StoreWeapon, StoreWeaponOutOfCombatDelayAmount);
}


void AWeapon::Attack()
{
	if (PawnOwner)
	{
		if (PawnOwner->CanAttack())
		{
			// if weapon currently equipped but stored, draw 
			if (!PawnOwner->bHasWeaponDrawn)
			{
				DrawWeapon();

				// in case heavy attack input resulted in draw instead of attack, clear flag in case next (and actually successful) attack input is light attack
				PawnOwner->bHeavyAttacking = false;
			}

			// if weapon currently equipped is drawn, attack
			else
			{
				PawnOwner->bAttacking = true;

				// reset store weapon out of combat timer
				GetWorldTimerManager().ClearTimer(StoreWeaponOutOfCombatTimer);
				GetWorldTimerManager().SetTimer(StoreWeaponOutOfCombatTimer, this, &AWeapon::StoreWeapon, StoreWeaponOutOfCombatDelayAmount);

				if (!PawnOwner->bCheatsOn)
				{
					// deduct stamina cost
					if (!PawnOwner->bHeavyAttacking)
					{ PawnOwner->Stamina -= WeaponConfig.LightAttackStaminaCost; }
					
					else
					{ PawnOwner->Stamina -= WeaponConfig.HeavyAttackStaminaCost; }

					// briefly delay stamina recovery
					PawnOwner->bCanRegenStamina = false;
					GetWorldTimerManager().SetTimer(PawnOwner->StaminaRegenDelayTimer, PawnOwner, &AMain::ResetCanRegenStamina, PawnOwner->AttackingStaminaRegenDelayAmount);
				}

				FWeaponAnim AnimToPlay;

				// light attack
				if (!PawnOwner->bHeavyAttacking)
				{
					if (PawnOwner->MovementStatus == EMovementStatus::EMS_Sprinting)
					{
						AnimToPlay = RunningLightAttackAnim;
						bPlayingRunningLightAttackAnim = true;
						PawnOwner->bShieldAttacking = true;
					}

					else
					{
						if (bLightAttackFollowupWindowOpen)
						{ LightAttackAnimationCounter += 1; }

						else
						{ LightAttackAnimationCounter = 0; }

						switch (LightAttackAnimationCounter)
						{
						case 0:
							AnimToPlay = LightAttackAnim1;
							break;

						case 1:
							AnimToPlay = LightAttackAnim2;
							break;

						case 2:
							AnimToPlay = LightAttackAnim3;
							LightAttackAnimationCounter = -1;
							break;

						default:
							;
						}

						bPlayingLightAttackAnim = true;
					}

					// play animations
					PlayWeaponAnimation(AnimToPlay);
					bPlayingAttackAnim = true;
				}

				// heavy attack
				else
				{
					if (PawnOwner->MovementStatus == EMovementStatus::EMS_Sprinting)
					{
						AnimToPlay = RunningHeavyAttackAnim;
						bPlayingRunningHeavyAttackAnim = true;

						// play animations
						PlayWeaponAnimation(AnimToPlay);
						bPlayingAttackAnim = true;
					}

					else
					{
						if (bHeavyAttackFollowupWindowOpen)
						{ HeavyAttackAnimationCounter += 1; }

						else
						{ HeavyAttackAnimationCounter = 0; }

						switch (HeavyAttackAnimationCounter)
						{
						case 0:

							HandleHeavyAttackBP();
							break;

						case 1:

							AnimToPlay = HeavyAttackAnim2;
							HeavyAttackAnimationCounter = -1;
							bPlayingHeavyAttackAnim = true;

							// play animations
							PlayWeaponAnimation(AnimToPlay);
							bPlayingAttackAnim = true;
							break;

						default:
							;
						}
					}
				}
			}
		}

		else
		{
			// if heavy attack input is rejected due to insufficient stamina, still clear flag so next valid attack isn't also a heavy if input is actually for light attack
			PawnOwner->bHeavyAttacking = false;
		}
	}
}


void AWeapon::AttackEnd()
{
	PawnOwner->bShouldPlayWeightShiftSoundWhenBlockBegins = true;
	GetWorldTimerManager().SetTimer(TimerHandle_WeightShiftSFXWindow, this, &AWeapon::ResetWeightShiftSFXWindow, 0.75f);

	PawnOwner->bAttacking = false;
	PawnOwner->bHeavyAttacking = false;
	PawnOwner->bShieldAttacking = false;
	PawnOwner->bCanJump = true;
	PawnOwner->SetInterpToEnemy(false);
	PawnOwner->EnableAttackRootMotionBP();
	PawnOwner->bCanMove = true;

	if (PawnOwner->bBlocking)
	{
		// ensure blocking stance resumes after anim has played
		PawnOwner->BlockUp();
		PawnOwner->BlockDown();
	}

	else
	{ PawnOwner->bBlocking = false; }
}


void AWeapon::OnAttackAnimationEnd()
{
	bPlayingAttackAnim = false;
	bPlayingLightAttackAnim = false;
	bPlayingHeavyAttackAnim = false;
	bPlayingRunningLightAttackAnim = false;
	bPlayingRunningHeavyAttackAnim = false;
}


void AWeapon::HandleAttacking()
{
	if (CanAttack())
	{
		if (PawnOwner)
		{
			Attack();
		}
	}

	LastAttackTime = GetWorld()->GetTimeSeconds();
}


void AWeapon::OnAttackStarted()
{
	HandleAttacking();
}


void AWeapon::OnAttackFinished()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_HandleAttacking);
	bReattacking = false;
}


void AWeapon::SetWeaponState(EWeaponState NewState)
{
	const EWeaponState PrevState = CurrentState;

	if (PrevState == EWeaponState::Attacking && NewState != EWeaponState::Attacking)
	{ OnAttackFinished(); }

	CurrentState = NewState;

	if (PrevState != EWeaponState::Attacking && NewState == EWeaponState::Attacking)
	{ OnAttackStarted(); }
}


void AWeapon::DetermineWeaponState()
{
	EWeaponState NewState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		if ((bWantsToAttack == true) && (CanAttack() == true))
		{ NewState = EWeaponState::Attacking; }
	}

	else if (bPendingEquip)
	{ NewState = EWeaponState::Equipping; }

	SetWeaponState(NewState);
}


void AWeapon::AttachMeshToPawn()
{
	if (PawnOwner)
	{
		USkeletalMeshComponent* PawnMesh = PawnOwner->GetMesh();
		AttachToComponent(PawnOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocketStored);
	}
}


UAudioComponent* AWeapon::PlayWeaponSound(USoundCue* Sound)
{
	UAudioComponent* AC = NULL;

	if (Sound && PawnOwner)
	{ AC = UGameplayStatics::SpawnSoundAttached(Sound, PawnOwner->GetRootComponent()); }

	return AC;
}


UAudioComponent* AWeapon::PlayLightSwingSound()
{
	UAudioComponent* AC = NULL;

	if (PawnOwner && LightAttackSwingSound1 && LightAttackSwingSound2)
	{
		USoundCue* SoundToPlay = nullptr;

		switch (LightSoundSelector)
		{
			case 0:
				SoundToPlay = LightAttackSwingSound1;
				LightSoundSelector += 1;
				break;

			case 1:
				SoundToPlay = LightAttackSwingSound2;
				LightSoundSelector = 0;
				break;

			default:
				;
		}

		if (SoundToPlay)
		{ AC = UGameplayStatics::SpawnSoundAttached(SoundToPlay, PawnOwner->GetEquippedWeapon()->GetWeaponMesh()); }
	}

	return AC;
}


UAudioComponent* AWeapon::PlayHeavySwingSound()
{
	UAudioComponent* AC = NULL;

	if (PawnOwner && HeavyAttackSwingSound1 && HeavyAttackSwingSound2)
	{
		USoundCue* SoundToPlay = nullptr;

		switch (HeavySoundSelector)
		{
		case 0:
			SoundToPlay = HeavyAttackSwingSound1;
			HeavySoundSelector += 1;
			break;

		case 1:
			SoundToPlay = HeavyAttackSwingSound2;
			HeavySoundSelector = 0;
			break;

		default:
			;
		}

		if (SoundToPlay)
		{ AC = UGameplayStatics::SpawnSoundAttached(SoundToPlay, PawnOwner->GetEquippedWeapon()->GetWeaponMesh()); }
	}

	return AC;
}


float AWeapon::PlayWeaponAnimation(const FWeaponAnim& Animation)
{
	float Duration = 0.0f;

	if (PawnOwner)
	{
		UAnimMontage* UseAnim = Animation.Pawn3P;

		if (UseAnim)
		{ Duration = PawnOwner->PlayAnimMontage(UseAnim);}
	}

	return Duration;
}


void AWeapon::StopWeaponAnimation(const FWeaponAnim& Animation)
{
	if (PawnOwner)
	{
		UAnimMontage* UseAnim = Animation.Pawn3P;

		if (UseAnim)
		{ PawnOwner->StopAnimMontage(UseAnim); }
	}
}

void AWeapon::ResetWeightShiftSFXWindow()
{
	PawnOwner->bShouldPlayWeightShiftSoundWhenBlockBegins = false;
}




