// © 2022 Andrew Creekmore 


#include "../World/EnemySpawn.h"
#include "../Enemies/Enemy.h"
#include "../Enemies/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/DataTable.h"

// sets default values
AEnemySpawn::AEnemySpawn()
{
	PrimaryActorTick.bCanEverTick = false;

	bShouldOverrideDefaultClassSettings = false;
	bShouldRespawnOnLoad = false;

	bActiveAI = false;
	bIsPatroller = false;

	if (EnemyDefaultsDataTable) // enemy subclass defaults (vary per enemy 'type' - archer, swordsman, etc)
	{ InitializeDefaultsFromDataTable(); }

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


void AEnemySpawn::PostInitProperties()
{
	if (EnemyDefaultsDataTable)
	{ InitializeDefaultsFromDataTable(); }

	Super::PostInitProperties();
}


void AEnemySpawn::PostLoad()
{
	if (EnemyDefaultsDataTable)
	{ InitializeDefaultsFromDataTable(); }

	Super::PostLoad();
}

#if WITH_EDITOR
void AEnemySpawn::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// if a new table is selected in the property editor, update the spawn's defaults accordingly
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AEnemySpawn, EnemyDefaultsDataTable))
	{
		if (EnemyDefaultsDataTable)
		{ InitializeDefaultsFromDataTable(); }
	}
}
#endif

// called when the game starts or when spawned
void AEnemySpawn::BeginPlay()
{
	Super::BeginPlay();
}

// called every frame
void AEnemySpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AEnemySpawn::InitializeSpawnedEnemy(APawn* SpawnedEnemy)
{
	if (AssignedAICombatDirector)
	{
		AEnemy* Enemy = Cast<AEnemy>(SpawnedEnemy);

		if (Enemy)
		{
			Enemy->bActiveAI = bActiveAI;
			Enemy->bIsPatroller = bIsPatroller;

			Enemy->PerceptionRange = PerceptionRange;
			Enemy->VisionRange = VisionRange;
			Enemy->VisionAggroRange = VisionAggroRange;
			Enemy->HearingAggroRange = HearingAggroRange;
			Enemy->TargetLossDelay = TargetLossDelay;

			Enemy->Damage = Damage;
			Enemy->Health = Health;
			Enemy->MaxHealth = MaxHealth;
			Enemy->DelayedHealthReportingValue = MaxHealth;
			Enemy->DelayedHealthBarValue = MaxHealth;
			Enemy->Poise = Poise;
			Enemy->MaxPoise = MaxPoise;
			Enemy->PoiseRecoveryDelay = PoiseRecoveryDelay;

			Enemy->MaxStunValue = MaxStunValue;
			Enemy->StunValueDrainRate = StunValueDrainRate;
			Enemy->StunRecoveryDelay = StunRecoveryDelay;
			Enemy->StaggerRecoveryDelay = StaggerRecoveryDelay;

			// set EnemyController variable + set isAlive Blackboard value
			Enemy->EnemyController = Cast<AEnemyController>(SpawnedEnemy->GetController());
			Enemy->EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("isAlive"), true);

			// add enemy to its assigned director's enemy list
			AssignedAICombatDirector->AssignedEnemyList.Add(SpawnedEnemy);

			// set base enemy defaults
			Enemy->BehaviorType = BehaviorType;
			Enemy->PrimaryAssignedZone = PrimaryAssignedZone;
			Enemy->AllowedNeighborZones = AllowedNeighborZones;

			// update tracking variables
			Enemy->SpawnPoint = this;
			bHasBeenSpawned = true;

			// set flag (Enemy class sets to false upon death) to respawn this enemy on next game load
			bShouldRespawnOnLoad = true;
		}
	}

	else
	{
		// screen log error message
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("No AICombatDirector assigned!")));
	}
}

void AEnemySpawn::InitializeDefaultsFromDataTable()
{
	static const FString ContextStringStats = (TEXT("Enemy Stats Data Context"));
	FEnemyStatDefaults* EnemyStatData = EnemyDefaultsDataTable->FindRow<FEnemyStatDefaults>(FName(TEXT("Defaults")), ContextStringStats, true);
	if (EnemyStatData && !bShouldOverrideDefaultClassSettings)
	{
		MaxHealth = EnemyStatData->MaxHealth;
		Health = EnemyStatData->MaxHealth;
		Damage = EnemyStatData->Damage;
		MaxPoise = EnemyStatData->MaxPoise;
		Poise = EnemyStatData->MaxPoise;
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


