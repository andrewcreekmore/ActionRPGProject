// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "../Enemies/AICombatDirector.h"
#include "../Enemies/Enemy.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "EnemySpawn.generated.h"



UCLASS()
class ACTIONRPGPROJECT_API AEnemySpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	
	// sets default values for this actor's properties
	AEnemySpawn();

	virtual void PostInitProperties() override;

	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)  override;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	TSubclassOf<class APawn> EnemyTypeToSpawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Spawn Settings")
	class UDataTable* EnemyDefaultsDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	class UBehaviorTree* EnemyBehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	AAICombatDirector* AssignedAICombatDirector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	TArray<int32> AssignedCheckpointKeys;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	EEnemyBehaviorType BehaviorType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 PrimaryAssignedZone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	TArray<int32> AllowedNeighborZones;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	bool bShouldOverrideDefaultClassSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	bool bHasBeenSpawned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	bool bShouldRespawnOnLoad;

	/**
	 *  enemy AI variable settings
	 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy AI")
	bool bActiveAI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy AI")
	bool bIsPatroller;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy AI")
	float PerceptionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy AI")
	float VisionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy AI")
	float VisionAggroRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy AI")
	float HearingAggroRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy AI")
	float TargetLossDelay;

	/**
	 *  enemy stats
	 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float Poise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float MaxPoise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float PoiseRecoveryDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float StaggerRecoveryDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float MaxStunValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float StunValueDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float StunRecoveryDelay;



protected:

	// called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void InitializeSpawnedEnemy(APawn* SpawnedEnemy);

	void InitializeDefaultsFromDataTable();

	/**
	* @param isTearingDown true if world is tearing down.
	*/UFUNCTION(BluePrintCallable, BlueprintPure, Category = "WorldState", meta = (DisplayName = "IsTearingDown", DefaultToSelf = caller, HidePin = caller))
	static void K2_IsTearingDown(UObject* caller, bool& isTearingDown)
	{
		isTearingDown = caller->GetWorld()->bIsTearingDown;
	}
};
