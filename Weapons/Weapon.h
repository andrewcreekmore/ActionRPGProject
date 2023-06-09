// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UAnimMontage;
class AMain;
class UAudioComponent;
class UParticleSystemComponent;
class UForceFeedbackEffect;
class USoundCue;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Idle,
	Attacking,
	Equipping,
	Drawn
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EMS_RustedSword UMETA(DisplayName = "RustedSword"),
	EMS_Broadsword UMETA(DisplayName = "Broadsword"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};


USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	// min time between two consecutive attacks
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponStats")
	float TimeBetweenAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStats")
	float LightAttackStaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStats")
	float HeavyAttackStaminaCost;

	// defaults
	FWeaponData()
	{
		TimeBetweenAttacks = 0.5f;
		LightAttackStaminaCost = 15.f;
		HeavyAttackStaminaCost = 25.f;
	}
};


USTRUCT(BlueprintType)
struct FHitConfiguration
{
	GENERATED_USTRUCT_BODY()

	/**A map of bone -> damage amount. If the bone is a child of the given bone, it will use this damage amount.
	A value of 2 would mean double damage etc */
	UPROPERTY(EditDefaultsOnly, Category = "Trace Info")
	TMap<FName, float> BoneDamageModifiers;

	// how far to trace for a hit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace Info")
	float Distance;

	// amount of damage to deal to actor when hit with standard light attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Info")
	float Damage;

	// heavy attack damage multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Info")
	float HeavyDamageMultiplier;

	// optional trace radius; a value of zero = line trace, while anything higher = sphere trace
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace Info")
	float WeaponRadius;

	UPROPERTY(EditDefaultsOnly, Category = "WeaponStats")
	TSubclassOf<UDamageType> DamageType;

	// defaults
	FHitConfiguration()
	{
		Distance = 1.0f;
		Damage = 10.0f;
		HeavyDamageMultiplier = 1.5f;
		WeaponRadius = 1.0f;
		DamageType = UDamageType::StaticClass();
	}
};


USTRUCT()
struct FWeaponAnim
{
	GENERATED_USTRUCT_BODY()

	// animation played in third person view
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* Pawn3P;
};


UCLASS()
class ACTIONRPGPROJECT_API AWeapon : public AActor
{
	GENERATED_BODY()

	friend class AMain;
	
public:	

	// sets default values for this actor's properties
	AWeapon();

	virtual void BeginPlay() override;

protected:
	
	// weapon is being equipped by owner pawn
	virtual void OnEquip();

	// weapon is now equipped by owner pawn
	virtual void OnEquipFinished();

	// weapon is stored/sheathed by owner pawn
	virtual void OnUnequip();

	// check if weapon is currently equipped
	bool IsEquipped() const;

	// check if mesh is already attached
	bool IsAttachedToPawn() const;

	/* input */

	virtual void StartAttack();

	virtual void StopAttack();

	bool CanAttack() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	EWeaponState GetCurrentState() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	USkeletalMeshComponent* GetWeaponMesh();

	UFUNCTION(BlueprintPure, Category = "Weapon")
	class AMain* GetPawnOwner() const;

	// gets the last time when this weapon was switched to
	float GetEquipStartedTime() const;

	// gets the duration of equipping weapon
	float GetEquipDuration() const;

	// the weapon item in the player's inventory
	UPROPERTY(BlueprintReadOnly)
	class UWeaponItem* Item;

	// pawn owner
	UPROPERTY()
	class AMain* PawnOwner;

	// weapon data
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType;

	FORCEINLINE EWeaponType GetWeaponType() { return WeaponType; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	FWeaponData WeaponConfig;

	// line trace data
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	FHitConfiguration HitConfig;


public:	

	// weapon mesh
	UPROPERTY(EditAnywhere, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;

	// name of the socket to attach to the character on when equipping (i.e., stored and visible on character but un-drawn)
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	FName AttachSocketStored;

	// name of the socket to attach to the character on when drawn (i.e., right hand)
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	FName AttachSocketDrawn;

	bool bIsCollisionActive;

	// attacking audio (swinging sound)
	UPROPERTY()
	UAudioComponent* SwingAC;

	// single light attack swing sounds
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* LightAttackSwingSound1;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* LightAttackSwingSound2;

	int32 LightSoundSelector;

	// single heavy attack swing sounds
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* HeavyAttackSwingSound1;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* HeavyAttackSwingSound2;

	int32 HeavySoundSelector;

	// on-equip sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float EquipSoundVolumeMultiplier;

	// on-unequip/sheath sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* UnequipSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float UnequipSoundVolumeMultiplier;

	// equip (not draw) animation, if any
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FWeaponAnim EquipAnim;
	
	// unequip (not sheathe) animation, if any
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FWeaponAnim UnequipAnim;

	// draw (unsheathe) animation
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FWeaponAnim DrawAnim;

	// store / unarm  animation
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FWeaponAnim StoreAnim;

	// store / unarm both weapon and equipped shield anim
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FWeaponAnim StorePlusShieldAnim;

	// light attack animations
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FWeaponAnim LightAttackAnim1;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FWeaponAnim LightAttackAnim2;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FWeaponAnim LightAttackAnim3;

	// heavy attack animations
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FWeaponAnim HeavyAttackAnim1;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FWeaponAnim HeavyAttackAnim2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* UnchargedHeavyAttackAnim1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* ChargedHeavyBegin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* ChargedHeavyHoldLoop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* ChargedHeavyRelease;

	// triggered when light attacking while sprinting
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FWeaponAnim RunningLightAttackAnim;

	// triggered when heavy attacking while sprinting
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FWeaponAnim RunningHeavyAttackAnim;

	/* flags */

	// is any attack animation playing?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 bPlayingAttackAnim : 1;

	// which kind of attack animation are we playing specifically?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 bPlayingLightAttackAnim : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 bPlayingHeavyAttackAnim : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayingRunningLightAttackAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayingRunningHeavyAttackAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLightAttackFollowupWindowOpen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHeavyAttackFollowupWindowOpen;

	// is weapon currently equipped?
	uint32 bIsEquipped : 1;

	// is weapon attack active?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint32 bWantsToAttack : 1;

	// is equip animation playing?
	uint32 bPendingEquip : 1;

	// is unequip animation playing?
	uint32 bPendingUnequip : 1;

	// weapon is re-attacking
	uint32 bReattacking;

	/** current weapon state */
	EWeaponState CurrentState;

	/**
	*  animation modifiers
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 LightAttackAnimationCounter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 HeavyAttackAnimationCounter;

	// time of last successful weapon attack
	float LastAttackTime;

	// last time this weapon was switched to
	float EquipStartedTime;

	// how much time weapon needs in order to be equipped
	float EquipDuration;

	FTimerHandle StoreWeaponOutOfCombatTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StoreWeaponOutOfCombatDelayAmount;

	// handle for efficient management of OnEquipFinished timer
	FTimerHandle TimerHandle_OnEquipFinished;

	// handle for efficient management of Attacking timer
	FTimerHandle TimerHandle_HandleAttacking;

	FTimerHandle TimerHandle_CheckForChargingHeavy;

	FTimerHandle TimerHandle_WeightShiftSFXWindow;

	/* weapon usage */

	UFUNCTION(BlueprintCallable)
	void DrawWeapon();

	UFUNCTION(BlueprintCallable)
	void StoreWeapon();

	UFUNCTION(BlueprintCallable)
	void ResetStoreWeaponTimer();

	virtual void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void OnAttackAnimationEnd();

	void HandleAttacking();

	virtual void OnAttackStarted();

	virtual void OnAttackFinished();

	// update weapon state
	void SetWeaponState(EWeaponState NewState);

	// determine current weapon state
	void DetermineWeaponState();

	// attaches weapon mesh to pawn's mesh
	void AttachMeshToPawn();

	/* weapon usage helpers*/

	// play weapon sounds
	UFUNCTION(BlueprintCallable)
	UAudioComponent* PlayWeaponSound(USoundCue* Sound);

	UFUNCTION(BlueprintCallable)
	UAudioComponent* PlayLightSwingSound();

	UFUNCTION(BlueprintCallable)
	UAudioComponent* PlayHeavySwingSound();

	// play weapon animations
	float PlayWeaponAnimation(const FWeaponAnim& Animation);

	// stop playing weapon animations
	void StopWeaponAnimation(const FWeaponAnim& Animation);

	UFUNCTION(BlueprintImplementableEvent)
	void HandleHeavyAttackBP();

	UFUNCTION(BlueprintImplementableEvent)
	void ReleaseChargedAttackBP();

	void ResetWeightShiftSFXWindow();

};