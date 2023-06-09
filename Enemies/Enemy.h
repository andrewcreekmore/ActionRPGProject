// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"


UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EMS_Idle			UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget	UMETA(DisplayName = "MoveToTarget"),
	EMS_Stalking		UMETA(DisplayName = "Stalking"),
	EMS_Attacking		UMETA(DisplayName = "Attacking"),
	EMS_Dead			UMETA(DisplayName = "Dead"),

	EMS_MAX				UMETA(DisplayName = "DefaultMAX")
};


UENUM(BlueprintType)
enum class EEnemyAwarenessLevel : uint8
{
	EMS_Passive			UMETA(DisplayName = "Passive"),
	EMS_Suspicious		UMETA(DisplayName = "Suspicious"),
	EMS_Alert			UMETA(DisplayName = "Alert"),
	EMS_Hostile			UMETA(DisplayName = "Hostile"),

	EMS_MAX				UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	EMS_Ghoul			UMETA(DisplayName = "Ghoul"),
	EMS_Wendigo			UMETA(DisplayName = "Wendigo"),
	EMS_Archer			UMETA(DisplayName = "Archer"),
	EMS_Swordsman		UMETA(DisplayName = "Swordsman"),
	EMS_Rogue			UMETA(DisplayName = "Rogue"),
	EMS_Warrior			UMETA(DisplayName = "Warrior"),

	EMS_MAX				UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EEnemyBehaviorType : uint8
{
	EMS_Engager			UMETA(DisplayName = "Engager"),
	EMS_Defender		UMETA(DisplayName = "Defender"),
	EMS_Ambusher		UMETA(DisplayName = "Ambusher"),

	EMS_MAX				UMETA(DisplayName = "DefaultMAX")
};


USTRUCT(BlueprintType)
struct FEnemyStatDefaults : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxPoise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PoiseRecoveryDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStunValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunValueDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunRecoveryDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StaggerRecoveryDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PerceptionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VisionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VisionAggroRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HearingAggroRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetLossDelay;
};

UCLASS()
class ACTIONRPGPROJECT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	
	// sets default values for this character's properties
	AEnemy();

	virtual void PostInitProperties() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EEnemyType EnemyType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UDataTable* EnemyDefaultsDataTable;

	FORCEINLINE EEnemyType GetEnemyType() { return EnemyType; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EEnemyBehaviorType BehaviorType;

	FORCEINLINE EEnemyBehaviorType GetBehaviorType() { return BehaviorType; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class AEnemySpawn* SpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	int32 PrimaryAssignedZone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<int32> AllowedNeighborZones;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bCurrentlyRelocating;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }
	FORCEINLINE EEnemyMovementStatus GetEnemyMovementStatus() { return EnemyMovementStatus; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EEnemyAwarenessLevel EnemyAwarenessLevel;

	UFUNCTION(BlueprintCallable)
	void SetEnemyAwarenessLevel(EEnemyAwarenessLevel AwarenessLevel); 

	FORCEINLINE EEnemyAwarenessLevel GetEnemyAwarenessLevel() { return EnemyAwarenessLevel; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* CombatRangeSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DistanceToPlayerCharacter;

	/**
	 *  enemy perception values
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bActiveAI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsPatroller;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float PerceptionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float VisionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float VisionAggroRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float HearingAggroRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float TargetLossDelay;

	/**
	 *  enemy stats
	 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DelayedHealthReportingValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	float DelayedHealthBarValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DelayedHealthBarDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Poise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxPoise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float StunValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxStunValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float StunValueDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float StunRecoveryDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float CanLookAtPlayerAfterExitingStunDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float ResetPlayFullVolumeHitSoundsDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float InterpToPlayerSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float TooCloseBackupVInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float TooCloseNoRootThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float TooCloseBackupThreshold;

	/**
	 *  movement modifiers
	 */
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float PassiveWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AlertWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bRotationDisabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FRotator PassiveRotationRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FRotator AlertRotationRate;

	/**
	 *   combat modifiers
	 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bStaggered;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bStunned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bLastReceivedHitWasHeavy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bRagdolled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bCanLookAtPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bCanBeExecuted;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bInterpToPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasValidTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FHitResult LastHitResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FName LastHitBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	AActor* SpawnedBloodPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bKnockdownAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bPushbackAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bHeavyPushbackAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bAirAttacking;

	/**
	 *  animation/sound/fx modifiers
	 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bPlayLowVolumeHitSound;

	FTimerHandle ResetPlayFullVolumeHitSoundsTimer;

	/**
	 *  animations & effects
	 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anims")
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anims")
	class UAnimMontage* StaggerMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anims")
	class UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anims")
	class UAnimMontage* DeathNoRagdollMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anims")
	class UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	TSubclassOf<UCameraShakeBase> HitImpactCameraShake;

	/**
	 *  sounds
	 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* HitSound1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* HitSound2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* HitSound3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* HitSoundLowVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* HeavyHitSound1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* HeavyHitSound2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* ShieldAttackHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	USoundCue* SwingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	USoundCue* DeathSound1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	USoundCue* DeathSound2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	float DeathSound1VolumeMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	float DeathSound2VolumeMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	float LowVolumeHitSoundsVolumeMultiplier;

	/**
	 *   combat
	 */

	// modify enemy health by either a positive or negative amount; return amount of health actually removed
	float ModifyHealth(const float Delta);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AMain* EnemyCombatTarget;

	FORCEINLINE void SetCombatTarget(AMain* Target) { EnemyCombatTarget = Target; }

	FRotator GetLookAtRotationYaw(FVector Target);

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMinTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMaxTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAlive;

	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DeathDespawnDelay;

	int32 AttackCounter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bCanTakeDamage;

	FTimerHandle TakeDamageTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float TakeDamageDelay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bPoiseBroken;

	FTimerHandle PoiseRecoveryTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float PoiseRecoveryDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float StaggerRecoveryDelay;

	int32 HitSoundToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bNextReceivedHitShouldKillEnemy;

protected:
	
	// called when the game starts or when spawned
	virtual void BeginPlay() override;

	// behavior tree for the enemy
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	UFUNCTION(BlueprintCallable)
	void SetStaggered(bool Staggered);

	void ResetStagger();

	void ResetStun();

	void ResetCanLookAtPlayer();

	void ResetFullVolumeHitSounds();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bShouldPlayPhysicalHitReactWhenNotPoiseBroken;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bInAttackRange;

	UFUNCTION(BlueprintCallable)
	void SetInAttackRange(bool InAttackRange);

	UPROPERTY(VisibleAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bCanAttack;

	FTimerHandle AttackWaitTimer;

	FTimerHandle ResetStaggerTimer;

	FTimerHandle ResetStunTimer;

	FTimerHandle ResetCanLookAtPlayerTimer;

	// minimum wait time between attacks
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AttackWaitTime;

	void ResetCanAttack();


public:	

	// called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void WhenDestroyed(AActor* Act);

	class AEnemyController* EnemyController;

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage(FName Section, float PlayRate);

	UFUNCTION()
	void CombatRangeSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void CombatRangeSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverlappingCombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AMain* CombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttacking;

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	// use inherited TakeDamage function (built-in)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintCallable)
	void Die(AActor* Causer);

	UFUNCTION(BlueprintCallable)
	void OnExecuted();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	UFUNCTION(BlueprintCallable)
	bool Alive();

	void Disappear();

	UFUNCTION(BlueprintCallable)
	void StaggerEnd();

	UFUNCTION(BlueprintCallable)
	void RecoverPoise();

	void DecrementHealth(float Amount);

	void SetCanTakeDamage();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayPhysicalHitReactBP();

	UFUNCTION(BlueprintImplementableEvent)
	void ApplyDeathblowImpulseBP();

	UFUNCTION(BlueprintImplementableEvent)
	void SetupRagdoll();

	UFUNCTION(BlueprintImplementableEvent)
	void DisableAttackRootMotionBP();

	UFUNCTION(BlueprintImplementableEvent)
	void EnableAttackRootMotionBP();

	UFUNCTION(BlueprintImplementableEvent)
	bool ValidateDeathAnimSpaceBP();
};
