// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shield.generated.h"

class UAnimMontage;
class AMain;
class UAudioComponent;
class UParticleSystemComponent;
class USoundCue;

UENUM(BlueprintType)
enum class EShieldState : uint8
{
	Idle,
	Blocking,
	Equipping,
	Drawn
};

UENUM(BlueprintType)
enum class EShieldType : uint8
{
	EMS_WoodenShield UMETA(DisplayName = "WoodenShield"),
	EMS_KiteShield UMETA(DisplayName = "KiteShield"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

USTRUCT(BlueprintType)
struct FShieldData
{
	GENERATED_USTRUCT_BODY()

		// min time between two consecutive block initializations
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ShieldStats")
		float TimeBetweenBeginBlocks;

		// damage when used offensively (during shield attack anims)
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ShieldStats")
		float ShieldAttackDamage;

		// how well the shield absorbs impacts (the higher the value, the lower the stamina cost when blocking hits)
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ShieldStats")
		float Stability;

	// defaults
	FShieldData()
	{
		TimeBetweenBeginBlocks = 0.25f;
		ShieldAttackDamage = 20.0f;
		Stability = 5.0f;
	}
};

USTRUCT()
struct FShieldAnim
{
	GENERATED_USTRUCT_BODY()

		// animation played in third person view
		UPROPERTY(EditDefaultsOnly, Category = "Animation")
		UAnimMontage* Pawn3P;
};

UCLASS()
class ACTIONRPGPROJECT_API AShield : public AActor
{
	GENERATED_BODY()

	friend class AMain;
	
public:	
	
	// sets default values for this actor's properties
	AShield();

	// called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:

	// shield is being equipped by owner pawn
	virtual void OnEquip();

	// shield is now equipped by owner pawn
	virtual void OnEquipFinished();

	// shield is stored by owner pawn
	virtual void OnUnequip();

	// check if shield is currently equipped
	bool IsEquipped() const;

	// check if mesh is already attached
	bool IsAttachedToPawn() const;

	/* input */

	virtual void StartBlock();

	virtual void StopBlock();

	bool CanBlock() const;

	UFUNCTION(BlueprintPure, Category = "Shield")
	EShieldState GetCurrentState() const;

	UFUNCTION(BlueprintPure, Category = "Shield")
	UStaticMeshComponent* GetShieldMesh();

	UFUNCTION(BlueprintPure, Category = "Shield")
	class AMain* GetPawnOwner() const;

	// gets the last time when this shield was switched to
	float GetEquipStartedTime() const;

	// gets the duration of equipping shield
	float GetEquipDuration() const;

	// the shield item in the player's inventory
	UPROPERTY(BlueprintReadOnly)
	class UShieldItem* Item;

	// shield data
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	FShieldData ShieldConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EShieldType ShieldType;


	// pawn owner
	UPROPERTY()
	class AMain* PawnOwner;



public:	

	// shield mesh
	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* ShieldMesh;

	// name of the socket to attach to the character on when equipping (i.e., stored and visible on character but un-drawn)
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	FName AttachSocketStored;

	// name of the socket to attach to the character on when drawn (i.e., left hand)
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	FName AttachSocketDrawn;

	// on-equip sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	float EquipSoundVolumeMultiplier;

	// on-unequip sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	USoundCue* UnequipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	bool bShouldPlayUnequipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	float UnequipSoundVolumeMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	class USoundCue* ShieldImpactSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	class USoundCue* ShieldGuardBrokenSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	USoundCue* WeightShiftSound;

	// equip (not draw) animation, if any
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FShieldAnim EquipAnim;

	// unequip (not store) animation, if any
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FShieldAnim UnequipAnim;

	// draw (unsheathe) animation
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FShieldAnim DrawAnim;

	// store / un-arm (sheath) animation
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FShieldAnim StoreAnim;

	// blocking stance animation
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FShieldAnim BlockingStanceAnim;

	/* flags */

	// is a blocking stance animation playing?
	uint32 bPlayingBlockAnim : 1;

	// is shield currently equipped?
	uint32 bIsEquipped : 1;

	// is shield block input active?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint32 bWantsToBlock : 1;

	// is equip animation playing?
	uint32 bPendingEquip : 1;

	// is unequip animation playing?
	uint32 bPendingUnequip : 1;

	// shield is re-blocking
	uint32 bReblocking;

	/** current shield state */
	EShieldState CurrentState;

	// time of last successful blocking stance initiation
	float LastBlockBeginTime;

	// last time this shield was switched to
	float EquipStartedTime;

	// how much time shield needs in order to be equipped
	float EquipDuration;

	// handle for efficient management of OnEquipFinished timer
	FTimerHandle TimerHandle_OnEquipFinished;

	// handle for efficient management of Blocking timer
	FTimerHandle TimerHandle_HandleBlocking;

	/* shield usage */

	virtual void Block();

	UFUNCTION(BlueprintCallable)
	void BlockEnd();

	UFUNCTION(BlueprintCallable)
	void OnBlockAnimationEnd();

	void HandleBlocking();

	virtual void OnBlockStarted();

	virtual void OnBlockFinished();

	// update shield state
	void SetShieldState(EShieldState NewState);

	// determine current shield state
	void DetermineShieldState();

	// attaches shield mesh to pawn's mesh
	void AttachMeshToPawn();

	/* shield usage helpers*/

	// play shield sounds
	UFUNCTION(BlueprintCallable)
	UAudioComponent* PlayShieldSound(USoundCue* Sound);

	// play shield animations
	float PlayShieldAnimation(const FShieldAnim& Animation);

	// stop playing shield animations
	void StopShieldAnimation(const FShieldAnim& Animation);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayShieldRaiseFX_BP();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayShieldLowerFX_BP();
};
