// © 2022 Andrew Creekmore 


#include "../Weapons/Shield.h"
#include "../Components/InventoryComponent.h"
#include "../Character/Main.h"
#include "../Character/MainPlayerController.h"
#include "../DebugMacros.h"
#include "../Enemies/Enemy.h"
#include "../Items/EquippableItem.h"
#include "../Items/ShieldItem.h"
#include "../Weapons/Weapon.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AShield::AShield()
{
	//ShieldMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetCollisionObjectType(ECC_WorldDynamic);
	ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShieldMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = ShieldMesh;

	ShieldType = EShieldType::EMS_MAX;

	bPlayingBlockAnim = false;
	bIsEquipped = false;
	bWantsToBlock = false;
	bPendingEquip = false;
	bPendingUnequip = false;
	CurrentState = EShieldState::Idle;
	AttachSocketStored = FName("Shield_Stored_Socket");
	AttachSocketDrawn = FName("WEAPON_L");
	LastBlockBeginTime = 0.0f;
	EquipSoundVolumeMultiplier = 1.f;
	UnequipSoundVolumeMultiplier = 1.f;
	bShouldPlayUnequipSound = true;
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
}


// called when the game starts or when spawned
void AShield::BeginPlay()
{
	Super::BeginPlay();

	PawnOwner = Cast<AMain>(GetOwner());
}


void AShield::OnEquip()
{
	AttachMeshToPawn();

	bPendingEquip = true;
	DetermineShieldState();

	GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinished, this, &AShield::OnEquipFinished, .5f);

	if (PawnOwner)
	{
		if (Item->bShouldNotifyOnInventoryAdd)
		{
			float realtimeSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());
			if (EquipSound && realtimeSeconds > 5.f) // don't play when auto-equipping on save game load
			{ UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation(), EquipSoundVolumeMultiplier); }
		}
	}
}


void AShield::OnEquipFinished()
{
	bIsEquipped = true;
	bPendingEquip = false;
	PawnOwner->bIsEquipping = false;
	PawnOwner->bCanEvade = true;

	DetermineShieldState();
}


void AShield::OnUnequip()
{
	bIsEquipped = false;
	PawnOwner->bIsEquipping = false;
	PawnOwner->bCanEvade = true;
	StopBlock();

	if (UnequipSound && bShouldPlayUnequipSound) // don't play unequip sound if another is being equipped in its place
	{ UGameplayStatics::PlaySoundAtLocation(this, UnequipSound, GetActorLocation(), UnequipSoundVolumeMultiplier); }

	// reset flag to default
	bShouldPlayUnequipSound = true;

	if (bPendingEquip)
	{
		StopShieldAnimation(EquipAnim);
		bPendingEquip = false;
		GetWorldTimerManager().ClearTimer(TimerHandle_OnEquipFinished);
	}

	DetermineShieldState();
}


bool AShield::IsEquipped() const
{
	return bIsEquipped;
}


bool AShield::IsAttachedToPawn() const
{
	return bIsEquipped || bPendingEquip;
}


void AShield::StartBlock()
{
	if (!bWantsToBlock)
	{
		bWantsToBlock = true;
		DetermineShieldState();
	}
}


void AShield::StopBlock()
{
	if (bWantsToBlock)
	{
		bWantsToBlock = false;
		DetermineShieldState();
	}

	BlockEnd();
}


bool AShield::CanBlock() const
{
	bool bCanBlock = PawnOwner != nullptr;
	bool bStateOKToBlock= ((CurrentState == EShieldState::Idle) || (CurrentState == EShieldState::Blocking));
	return ((bCanBlock == true) && (bStateOKToBlock == true));
}


EShieldState AShield::GetCurrentState() const
{
	return CurrentState;
}


UStaticMeshComponent* AShield::GetShieldMesh()
{
	return ShieldMesh;
}


class AMain* AShield::GetPawnOwner() const
{
	return PawnOwner;
}


float AShield::GetEquipStartedTime() const
{
	return EquipStartedTime;
}


float AShield::GetEquipDuration() const
{
	return EquipDuration;
}


void AShield::Block()
{
	if (PawnOwner)
	{
		if ((!PawnOwner->bBlocking) && (!PawnOwner->bIsDodging) && (!PawnOwner->bIsRolling) && (PawnOwner->Stamina > 0) && (PawnOwner->MovementStatus != EMovementStatus::EMS_Staggered) && (PawnOwner->MovementStatus != EMovementStatus::EMS_Dead))
		{
			// if shield currently equipped but stored, draw 
			if (!PawnOwner->bHasWeaponDrawn)
			{
				PlayShieldAnimation(DrawAnim);
				
				if (PawnOwner->EquippedWeapon == nullptr)
				{
					if (PawnOwner->bPlayingUnarmedAttackAnim) { return; }

					PawnOwner->bBlocking = true;
					PawnOwner->CurrentStaminaRegenRate = PawnOwner->DefaultStaminaRegenRate * PawnOwner->BlockingStaminaRegenRateModifier;

					// play animations
					FShieldAnim AnimToPlay = BlockingStanceAnim;
					PlayShieldAnimation(AnimToPlay);
					bPlayingBlockAnim = true;

					// if player is blocking right out of an attack anim (i.e., haven't blended back to neutral idle stance), play a 'weight shifting' SFX
					if (PawnOwner->bShouldPlayWeightShiftSoundWhenBlockBegins && WeightShiftSound)
					{
						UGameplayStatics::PlaySoundAtLocation(this, WeightShiftSound, PawnOwner->GetActorLocation(), 1.f);
					}
				}
			}

			// if shield currently equipped is drawn, block
			else
			{
				if (PawnOwner->EquippedWeapon != nullptr)
				{ if (PawnOwner->EquippedWeapon->bPlayingAttackAnim) { return; } }

				if (PawnOwner->EquippedWeapon == nullptr)
				{ if (PawnOwner->bPlayingUnarmedAttackAnim) { return; } }

				PawnOwner->bBlocking = true;
				PawnOwner->CurrentStaminaRegenRate = PawnOwner->DefaultStaminaRegenRate * PawnOwner->BlockingStaminaRegenRateModifier;

				// play animations
				FShieldAnim AnimToPlay = BlockingStanceAnim;
				PlayShieldAnimation(AnimToPlay);
				bPlayingBlockAnim = true;

				// if player is blocking right out of an attack anim (i.e., haven't blended back to neutral idle stance), play a 'weight shifting' SFX
				if (PawnOwner->bShouldPlayWeightShiftSoundWhenBlockBegins && WeightShiftSound)
				{	
					PawnOwner->bShouldPlayWeightShiftSoundWhenBlockBegins = false;
					PawnOwner->PlayBlockWeightShiftSFX();
				}

				else
				{ PlayShieldRaiseFX_BP();}
			}
		}
	}
}


void AShield::BlockEnd()
{
	StopShieldAnimation(BlockingStanceAnim);
	PawnOwner->bBlocking = false;
	PawnOwner->bCanJump = true;
	
	PlayShieldLowerFX_BP();
}


void AShield::OnBlockAnimationEnd()
{
	bPlayingBlockAnim = false;
}


void AShield::HandleBlocking()
{
	if (CanBlock())
	{
		if (PawnOwner)
		{
			Block();

			// setup re-blocking timer
			bReblocking = (CurrentState == EShieldState::Blocking && ShieldConfig.TimeBetweenBeginBlocks > 0.0f);
			if (bReblocking)
			{ GetWorldTimerManager().SetTimer(TimerHandle_HandleBlocking, this, &AShield::HandleBlocking, ShieldConfig.TimeBetweenBeginBlocks, false); }
		}
	}

	LastBlockBeginTime = GetWorld()->GetTimeSeconds();
}


void AShield::OnBlockStarted()
{
	// start blocking; can be delayed to satisfy TimeBetweenBlocks
	const float GameTime = GetWorld()->GetTimeSeconds();

	if (LastBlockBeginTime > 0  && ShieldConfig.TimeBetweenBeginBlocks > 0.0f && LastBlockBeginTime + ShieldConfig.TimeBetweenBeginBlocks > GameTime)
	{ GetWorldTimerManager().SetTimer(TimerHandle_HandleBlocking, this, &AShield::HandleBlocking, LastBlockBeginTime + ShieldConfig.TimeBetweenBeginBlocks - GameTime, false); }

	else
	{ HandleBlocking(); }
}


void AShield::OnBlockFinished()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_HandleBlocking);
	bReblocking = false;
}


void AShield::SetShieldState(EShieldState NewState)
{
	const EShieldState PrevState = CurrentState;

	if (PrevState == EShieldState::Blocking && NewState != EShieldState::Blocking)
	{ OnBlockFinished(); }

	CurrentState = NewState;

	if (PrevState != EShieldState::Blocking && NewState == EShieldState::Blocking)
	{ OnBlockStarted(); }
}


void AShield::DetermineShieldState()
{
	EShieldState NewState = EShieldState::Idle;

	if (bIsEquipped)
	{
		if ((bWantsToBlock == true) && (CanBlock() == true))
		{ NewState = EShieldState::Blocking; }
	}

	else if (bPendingEquip)
	{ NewState = EShieldState::Equipping; }

	SetShieldState(NewState);
}


void AShield::AttachMeshToPawn()
{
	if (PawnOwner)
	{
		USkeletalMeshComponent* PawnMesh = PawnOwner->GetMesh();
		AttachToComponent(PawnOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocketDrawn);
	}
}


UAudioComponent* AShield::PlayShieldSound(USoundCue* Sound)
{
	UAudioComponent* AC = NULL;

	if (Sound && PawnOwner)
	{ AC = UGameplayStatics::SpawnSoundAttached(Sound, PawnOwner->GetRootComponent()); }

	return AC;
}


float AShield::PlayShieldAnimation(const FShieldAnim& Animation)
{
	float Duration = 0.0f;

	if (PawnOwner)
	{
		UAnimMontage* UseAnim = Animation.Pawn3P;

		if (UseAnim)
		{ Duration = PawnOwner->PlayAnimMontage(UseAnim); }
	}

	return Duration;
}


void AShield::StopShieldAnimation(const FShieldAnim& Animation)
{
	if (PawnOwner)
	{
		UAnimMontage* UseAnim = Animation.Pawn3P;

		if (UseAnim)
		{ PawnOwner->StopAnimMontage(UseAnim); }
	}
}

