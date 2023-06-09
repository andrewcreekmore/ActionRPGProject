// © 2022 Andrew Creekmore 


#include "Enemy.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "../DebugMacros.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ActionRPGProject/Character/Main.h"
#include "ActionRPGProject/Character/MainPlayerController.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../World//EnemySpawn.h"

// sets default values
AEnemy::AEnemy()
{
 	// set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	// attack initiation range
	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());
	CombatRangeSphere->InitSphereRadius(100.f);

	bOverlappingCombatSphere = false;

	EnemyType = EEnemyType::EMS_MAX;
	BehaviorType = EEnemyBehaviorType::EMS_Engager;

	PrimaryAssignedZone = 0;

	// default statuses
	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;
	EnemyAwarenessLevel = EEnemyAwarenessLevel::EMS_Passive;
	bIsAlive = true;
	bRagdolled = false;
	bActiveAI = true;

	// movement defaults
	PassiveWalkSpeed = 125.f;
	AlertWalkSpeed = 200.f;
	RunningSpeed = 375.f;
	GetCharacterMovement()->MaxWalkSpeed = PassiveWalkSpeed;
	PassiveRotationRate = FRotator(0.f, 90.f, 0.f);
	AlertRotationRate = FRotator(0.f, 135.f, 0.f);
	GetCharacterMovement()->RotationRate = PassiveRotationRate;

	// enemy stat defaults
	MaxHealth = 100.f;
	Health = MaxHealth;
	DelayedHealthReportingValue = MaxHealth;
	DelayedHealthBarValue = MaxHealth;
	DelayedHealthBarDrainRate = 20.f;
	
	Damage = 20.f;
	MaxPoise = 30.f;
	Poise = MaxPoise;
	bPoiseBroken = false;
	PoiseRecoveryDelay = 3.f;
	StaggerRecoveryDelay = 0.75f;
	bShouldPlayPhysicalHitReactWhenNotPoiseBroken = true;

	bStunned = false;
	MaxStunValue = 75.f;
	StunValue = 0.f;
	StunValueDrainRate = 5.f;
	StunRecoveryDelay = 4.f;
	CanLookAtPlayerAfterExitingStunDelay = 0.5f;

	TakeDamageDelay = 0.35f;

	AttackMinTime = 0.25f;
	AttackMaxTime = 0.75f;

	HitSoundToPlay = 0;
	DeathSound1VolumeMultiplier = 1.0f;
	DeathSound2VolumeMultiplier = 1.0f;

	// behavior modifiers
	bCanBeExecuted = false;
	bCanLookAtPlayer = true;
	bLastReceivedHitWasHeavy = false;
	InterpToPlayerSpeed = 15.f;
	bInterpToPlayer = false;
	bNextReceivedHitShouldKillEnemy = false;
	bKnockdownAttacking = false;
	bPushbackAttacking = false;
	bHeavyPushbackAttacking = false;
	bAirAttacking = false;

	DistanceToPlayerCharacter = 0.0f;
	TooCloseNoRootThreshold = 135.f;
	TooCloseBackupThreshold = 100.f;
	TooCloseBackupVInterpSpeed = 3.f;

	DeathDespawnDelay = 10.0f;
	AttackCounter = 0;
	AttackWaitTime = 2.0f;

	bCurrentlyRelocating = false;
	bInAttackRange = false;
	bHasValidTarget = false;
	bStaggered = false;
	bCanTakeDamage = true;
	bIsPatroller = false;
	bCanAttack = true;
	bRotationDisabled = false;

	bPlayLowVolumeHitSound = false;
	ResetPlayFullVolumeHitSoundsDelay = 1.f;
	LowVolumeHitSoundsVolumeMultiplier = 0.5f;

	// perception defaults; instance editable

	/* overall perception range. includes both vision and hearing. outside of this you don't exist.
	(used on initial broad sphere test before filtering for line of sight/facing orientation, noise distance, aggro ranges etc) */
	PerceptionRange = 2000.0f;

	/* after filtering for both line of sight and facing orientation relative to target, check that target
	is within this before returning successful visual perception. by default, slightly less than overall perception range */
	VisionRange = 1500.0f;

	/* only applies once, to the initial visual perception of the player, and determines if allowed to switch from Passive to Hostile.
	does not reset/trigger again, so once aggro-ed, enemy tracks based purely on PerceptionRange/VisionRange. by default, same as vision range */
	VisionAggroRange = VisionRange;

	/* only applies once, to the initial aural perception of the player, and determines if allowed to switch from Passive to Suspicious.
	by default, same as overall perception range */
	HearingAggroRange = PerceptionRange;

	/* how long after currently tracked player target breaks vision range and/or line of sight before enemy 
	loses track and switches to searching last seen location (or last heard location, if more recent data point) */
	TargetLossDelay = 8.0f;
}

void AEnemy::PostInitProperties()
{
	if (EnemyDefaultsDataTable)
	{
		static const FString ContextStringStats = (TEXT("Enemy Stats Data Context"));
		FEnemyStatDefaults* EnemyStatData = EnemyDefaultsDataTable->FindRow<FEnemyStatDefaults>(FName(TEXT("Defaults")), ContextStringStats, true);
		if (EnemyStatData)
		{
			MaxHealth = EnemyStatData->MaxHealth;
			Health = MaxHealth;
			Damage = EnemyStatData->Damage;
			MaxPoise = EnemyStatData->MaxPoise;
			Poise = MaxPoise;
			PoiseRecoveryDelay = EnemyStatData->PoiseRecoveryDelay;
			MaxStunValue = EnemyStatData->MaxStunValue;
			StunValueDrainRate = EnemyStatData->StunValueDrainRate;
			StunRecoveryDelay = EnemyStatData->StunRecoveryDelay;
			StaggerRecoveryDelay = EnemyStatData->StaggerRecoveryDelay;
			PerceptionRange = EnemyStatData->PerceptionRange;
			VisionRange = EnemyStatData->VisionRange;
			VisionAggroRange = EnemyStatData->VisionAggroRange;
			HearingAggroRange = EnemyStatData->HearingAggroRange;
			TargetLossDelay = EnemyStatData->TargetLossDelay;
		}
	}

	Super::PostInitProperties();
}

void AEnemy::SetEnemyAwarenessLevel(EEnemyAwarenessLevel AwarenessLevel)
{
	EnemyAwarenessLevel = AwarenessLevel;
}


// called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// get the AI controller
	EnemyController = Cast<AEnemyController>(GetController());

	// also set the blackboard key default: CanAttack = true; isAlive = true
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("isAlive"), true);
	}

	CombatRangeSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &AEnemy::CombatRangeSphereOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddUniqueDynamic(this, &AEnemy::CombatRangeSphereOverlapEnd);

	// keep enemy meshes/capsules from colliding with camera
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECR_Ignore);

	// initialize behavior tree
	if (EnemyController)
	{ EnemyController->RunBehaviorTree(BehaviorTree); }
}

// called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// determine how much red "delayed" health to drain after delay until meeting regular current health value (health bar effect)
	float DeltaDelayedHealthBarDrain = DelayedHealthBarDrainRate * DeltaTime;
	if (DelayedHealthBarValue > DelayedHealthReportingValue)
	{ DelayedHealthBarValue -= DeltaDelayedHealthBarDrain; }

	// determine how much of the stun value meter to reduce per frame until back to zero
	float DeltaStunValueDrain = StunValueDrainRate * DeltaTime;
	if (!bStunned && StunValue > 0) { StunValue -= DeltaStunValueDrain; }
	if (StunValue < 0) { StunValue = 0; }

	// soft lock-on to player during attack anims (bInterpToPlayer set/unset via BT/AnimBP notifies)
	if (bInterpToPlayer && CombatTarget && !bStunned && !bStaggered)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpToPlayerSpeed);

		SetActorRotation(InterpRotation);
	}
}


// removes enemy ref from its assigned AICombatDirector's enemy list upon destruction
void AEnemy::WhenDestroyed(AActor* Act)
{
	if (SpawnPoint)
	{
		if (SpawnPoint->AssignedAICombatDirector)
		{ SpawnPoint->AssignedAICombatDirector->AssignedEnemyList.Remove(this); }
	}
}

FRotator AEnemy::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}


void AEnemy::CombatRangeSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) { return; }

	if (OtherActor && Alive())
	{
		auto Main = Cast<AMain>(OtherActor);

		if (Main)
		{
			bInAttackRange = true;

			if (EnemyController)
			{ EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true); }

			// enemy-side flag/value setting
			bHasValidTarget = true;
			CombatTarget = Main;
			bOverlappingCombatSphere = true;
		}
	}
}


void AEnemy::CombatRangeSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) { return; }

	auto Main = Cast<AMain>(OtherActor);

	if (Main)
	{
		bInAttackRange = false;

		if (EnemyController)
		{ EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false); }
	}
}


void AEnemy::PlayAttackMontage(FName Section, float PlayRate)
{
	if (Alive() && bHasValidTarget && CombatTarget->Alive())
	{
		if (EnemyController)
		{
			EnemyController->StopMovement();
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
		}

		if (!bAttacking && !bStaggered)
		{
			bAttacking = true;

			// also set the corresponding blackboard key, for behavior tree access
			if (EnemyController)
			{ EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("Attacking"), true); }

			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			if (AnimInstance && AttackMontage)
			{
				AnimInstance->Montage_Play(AttackMontage);
				AnimInstance->Montage_JumpToSection(Section, AttackMontage);
			}
		}
	}
}


void AEnemy::AttackEnd()
{
	bAttacking = false;
	bRotationDisabled = false;
	EnableAttackRootMotionBP();

	// also set the corresponding blackboard key, for behavior tree access
	if (EnemyController)
	{ EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("Attacking"), false); }
}


// only for use outside of player combat (i.e., explosive item damage)
void AEnemy::DecrementHealth(float Amount)
{
	Health = Health -= Amount;
}


// for preventing double hits from a single player attack
void AEnemy::SetCanTakeDamage()
{
	bCanTakeDamage = true;
}


void AEnemy::SetStaggered(bool Staggered)
{
	bStaggered = Staggered;

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Staggered"), Staggered);
		// keep enemy from rotating to focus on player during stagger / hit react anims
		EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
	}
}


void AEnemy::ResetStagger()
{
	bStaggered = false;

	if (EnemyController)
	{ EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Staggered"), false); }
}


void AEnemy::ResetStun()
{
	bStunned = false;

	StunValue = 0.f;

	if (EnemyController)
	{ EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Stunned"), false); }

	GetWorldTimerManager().SetTimer(ResetCanLookAtPlayerTimer, this, &AEnemy::ResetCanLookAtPlayer, CanLookAtPlayerAfterExitingStunDelay);
}


void AEnemy::ResetCanLookAtPlayer()
{
	bCanLookAtPlayer = true;
}


void AEnemy::ResetFullVolumeHitSounds()
{
	bPlayLowVolumeHitSound = false;
}


void AEnemy::SetInAttackRange(bool InAttackRange)
{
	bInAttackRange = InAttackRange;

	if (EnemyController)
	{ EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), InAttackRange); }
}


float AEnemy::ModifyHealth(const float Delta)
{
	const float OldHealth = Health;
	Health = FMath::Clamp<float>(Health + Delta, 0.0f, MaxHealth);

	return Health - OldHealth;
}


float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (bCanTakeDamage)
	{
		// in case enemy is currently attacking and has been interrupted by taking damage, ensure we still call AttackEnd() (otherwise triggered by attack anim notify event)
		AttackEnd();

		// take health and poise damage
		if (!bStunned)
		{
			const float DamageDealt = ModifyHealth(-DamageAmount);
			Poise -= DamageAmount;
		}

		// if occurs during stun sequence and execution didn't trigger (bad elevation match, etc), die
		if (bStunned)
		{
			Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

			// play camera shake
			if (HitImpactCameraShake)
			{ GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(HitImpactCameraShake, 1.0f); }

			UGameplayStatics::PlaySoundAtLocation(this, HitSound1, GetActorLocation(), 1.f, 1.0f, 0.0f);
			ModifyHealth(-Health);
			Die(DamageCauser);

			return 0.0f;
		}


		// take stun damage
		if (!bStunned && StunValue < MaxStunValue && Health > 0.0f)
		{
			StunValue += DamageAmount;

			if (StunValue >= MaxStunValue)
			{
				StunValue = MaxStunValue;

				// set as stunned + set delay timer for reset
				bStunned = true;
				bCanLookAtPlayer = false;
				bCanBeExecuted = true;

				bPlayLowVolumeHitSound = true;
				GetWorldTimerManager().SetTimer(ResetPlayFullVolumeHitSoundsTimer, this, &AEnemy::ResetFullVolumeHitSounds, ResetPlayFullVolumeHitSoundsDelay);

				if (EnemyController)
				{ EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Stunned"), true); }

				GetWorldTimerManager().SetTimer(ResetStunTimer, this, &AEnemy::ResetStun, StunRecoveryDelay);

				bCanTakeDamage = false;
				GetWorldTimerManager().SetTimer(TakeDamageTimer, this, &AEnemy::SetCanTakeDamage, TakeDamageDelay);
			}
		}

		// play hit sound fx
		auto Main = Cast<AMain>(DamageCauser);

		if (Main)
		{
			float VolumeToUse = 1.0f;
			if (bPlayLowVolumeHitSound)
			{ VolumeToUse = LowVolumeHitSoundsVolumeMultiplier; }

			if (Main->bShieldAttacking)
			{
				// play shield hit sound fx
				if (ShieldAttackHitSound)
				{ UGameplayStatics::PlaySoundAtLocation(this, ShieldAttackHitSound, GetActorLocation(), VolumeToUse, 1.0f, 0.0f); }
			}

			if (Main->bHeavyAttacking)
			{
				bLastReceivedHitWasHeavy = true;

				// play heavy hit sound fx (two sounds, layered)
				if (HeavyHitSound1 && HeavyHitSound2)
				{
					UGameplayStatics::PlaySoundAtLocation(this, HeavyHitSound1, GetActorLocation(), VolumeToUse, 1.0f, 0.0f);
					UGameplayStatics::PlaySoundAtLocation(this, HeavyHitSound2, GetActorLocation(), VolumeToUse, 1.0f, 0.0f);
				}
			}

			else if (!Main->bShieldAttacking && !Main->bHeavyAttacking)
			{
				if (HitSound1 && HitSound2 && HitSound3)
				{
					switch (HitSoundToPlay)
					{
					case 0:
						UGameplayStatics::PlaySoundAtLocation(this, HitSound1, GetActorLocation(), VolumeToUse, 1.0f, 0.0f);
						HitSoundToPlay += 1;
						break;

					case 1:
						UGameplayStatics::PlaySoundAtLocation(this, HitSound2, GetActorLocation(), VolumeToUse, 1.0f, 0.0f);
						HitSoundToPlay += 1;
						break;

					case 2:
						UGameplayStatics::PlaySoundAtLocation(this, HitSound3, GetActorLocation(), VolumeToUse, 1.0f, 0.0f);
						HitSoundToPlay = 0;
						break;

					default:
						;
					}
				}
			}
		
			// play camera shake
			if (HitImpactCameraShake)
			{
				GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(HitImpactCameraShake, 1.0f);
			}

			// poise broken?
			if (Poise <= 0.f)
			{
				// break: enter staggered state + play anim (anim + particle FX triggered in BP)
				bPoiseBroken = true;
				SetStaggered(true);

				GetWorldTimerManager().SetTimer(ResetStaggerTimer, this, &AEnemy::ResetStagger, StaggerRecoveryDelay);
				GetWorldTimerManager().SetTimer(PoiseRecoveryTimer, this, &AEnemy::RecoverPoise, PoiseRecoveryDelay);
			}

			// call parent function (blueprint: play blood fx + directional hit reaction anim, etc)
			Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

			bCanTakeDamage = false;
			GetWorldTimerManager().SetTimer(TakeDamageTimer, this, &AEnemy::SetCanTakeDamage, TakeDamageDelay);

			// deathblow?
			if (Health <= 0.f)
			{
				if (EnemyType != EEnemyType::EMS_Warrior)
				{
					Die(DamageCauser);
					return 0.0f;
				}

				else // boss (Warrior) enters stunned state to be executed
				{
					// set as stunned + set delay timer for reset
					bStunned = true;
					bCanLookAtPlayer = false;

					if (EnemyController)
					{ EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Stunned"), true); }

					GetWorldTimerManager().SetTimer(ResetStunTimer, this, &AEnemy::ResetStun, StunRecoveryDelay);
					return 0.0f;
				}
			}

			// whether poise broken or not, play physical hit react at minimum (if not already dead; in that case, deathblow impulse react is triggered BP side)
			if (bPoiseBroken || bShouldPlayPhysicalHitReactWhenNotPoiseBroken)
			{ PlayPhysicalHitReactBP(); }

			return DamageAmount;
		}
	}

	return 0.0f;
}


void AEnemy::Die(AActor* Causer)
{
	// change status
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);
	bIsAlive = false;
	SetEnemyAwarenessLevel(EEnemyAwarenessLevel::EMS_Passive);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	// any obstructions to canned death anim?
	bool bDeathSpaceClear = ValidateDeathAnimSpaceBP();

	if (bDeathSpaceClear && DeathNoRagdollMontage)
	{
		if (AnimInstance)
		{ AnimInstance->Montage_Play(DeathNoRagdollMontage, 1.0f); }
	}

	// apply deathblow impulse and play death anim with transition into ragdoll
	else
	{
		if (AnimInstance && DeathMontage) // if both are valid
		{ AnimInstance->Montage_Play(DeathMontage, 1.0f); }

		SetupRagdoll();
		ApplyDeathblowImpulseBP();
	}

	if (SpawnPoint)
	{ SpawnPoint->bShouldRespawnOnLoad = false; }

	// clear focus + set Blackboard isAlive key to false
	if (EnemyController)
	{
		EnemyController->SetFocus(NULL);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("isAlive"), false);
	}

	bAttacking = false;
}


void AEnemy::OnExecuted()
{
	// change status
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);
	bIsAlive = false;
	SetEnemyAwarenessLevel(EEnemyAwarenessLevel::EMS_Passive);

	if (SpawnPoint)
	{ SpawnPoint->bShouldRespawnOnLoad = false; }

	// clear focus + set Blackboard isAlive key to false
	if (EnemyController)
	{
		EnemyController->SetFocus(NULL);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("isAlive"), false);
	}

	bAttacking = false;
}


void AEnemy::DeathEnd()
{
	// turn off collision volumes
	CombatRangeSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool AEnemy::Alive()
{
	return (GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead);
}

void AEnemy::Disappear()
{
	Destroy();
}

void AEnemy::StaggerEnd()
{
	ResetStagger();
	AttackEnd();
}


void AEnemy::RecoverPoise()
{
	bPoiseBroken = false;
	SetStaggered(false);
	Poise = MaxPoise;
}


void AEnemy::ResetCanAttack()
{
	bCanAttack = true;

	// also set the blackboard key
	if (EnemyController)
	{ EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true); }
}