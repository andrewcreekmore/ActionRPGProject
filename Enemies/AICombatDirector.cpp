// © 2022 Andrew Creekmore 


#include "AICombatDirector.h"
#include "../DebugMacros.h"
#include "../Enemies/Enemy.h"

// sets default values
AAICombatDirector::AAICombatDirector()
{
	PrimaryActorTick.bCanEverTick = true;

	PositionRelativeToCameraWeight = 1.f;
	AttackRecencyWeight = 1.f;
	DelayBetweenIssuingAttacks = 4.0f;
}

// called when the game starts or when spawned
void AAICombatDirector::BeginPlay()
{
	Super::BeginPlay();
}

// called every frame
void AAICombatDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


TArray<float> AAICombatDirector::RecalculateCombatGrading(APawn* EnemyInstance)
{
	// after CurrentlyHostileEnemyList is updated in BP:

	// local array of all intermediate grades (for debug)
	TArray<float> AllGradeResults;

	// set enemy's base grade value at 1 and put into a temp local newCombatGrade
	float NewCombatGrade = 1.f;

	// get enemy's grade value based on their position relative to the camera
	float PositionRelativeToCameraGrade = GradeByPositionRelativeToCamera(EnemyInstance);
	// store intermediate grade in array for debug
	AllGradeResults.Add(PositionRelativeToCameraGrade);
	// multiply weighted grade value with newCombatGrade
	NewCombatGrade = NewCombatGrade * PositionRelativeToCameraGrade;

	// get enemy's grade value based on how recently they attacked
	float AttackRecencyGrade = GradeByAttackRecency(EnemyInstance);
	// store intermediate grade in array for debug
	AllGradeResults.Add(AttackRecencyGrade);
	// multiply weighted value with newCombatGrade
	NewCombatGrade = NewCombatGrade * AttackRecencyGrade;

	AllGradeResults.Add(NewCombatGrade);

	return AllGradeResults;
}

float AAICombatDirector::GradeByPositionRelativeToCamera(APawn* EnemyInstance)
{
	float NewPositionGrade = 0.0f;

	if (EnemyInstance)
	{
		// get enemy's forward vector
		FVector EnemyForwardVector = EnemyInstance->GetActorForwardVector();

		// get camera's forward vector
		APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
		FVector CameraForwardVector = CameraManager->GetCameraRotation().Vector();

		// check dot product between forward vectors
		// DotProduct > 0.0f same direction
		// DotProduct == 0.0f perpendicular direction
		// DotProduct < 0.0f opposite direction
		float DotProduct = FVector::DotProduct(EnemyForwardVector, CameraForwardVector);
		float MultbyNeg1Result = DotProduct * -1.f;
		float NormalizedResult = UKismetMathLibrary::NormalizeToRange(MultbyNeg1Result, -1.0, 1.0);
		float WeightedResult = NormalizedResult * PositionRelativeToCameraWeight;

		NewPositionGrade += WeightedResult;
	}

	return NewPositionGrade;
}

float AAICombatDirector::GradeByAttackRecency(APawn* EnemyInstance)
{
	float NewRecencyGrade = 0.0f;

	if (EnemyInstance)
	{
		// sort the EnemyAttackRecency map by largest (i.e., most recent in game time seconds) Value
		EnemyAttackRecencyMap.ValueSort([](float A, float B) {
			return A > B;
			});

		int32 LocalRank = 0;

		// indices of this map now serve as a ranking, with the enemy with the lowest timestamp (i.e., attacked LEAST recently) at the highest "rank" (n-1)
		// find passed current enemy instance in the now-sorted map; their index is their rank/grade
		for (auto& EnemyGrade : EnemyAttackRecencyMap)
		{
			// rank result will be from 1 to n
			LocalRank += 1;

			if (EnemyGrade.Key == EnemyInstance)
			{ break; }
		}

		// normalize and apply weight
		float NormalizedResult = UKismetMathLibrary::NormalizeToRange(float(LocalRank), 0.0, 3.0);
		float WeightedResult = NormalizedResult * AttackRecencyWeight;

		NewRecencyGrade += WeightedResult;
	}

	return NewRecencyGrade;
}

APawn* AAICombatDirector::FindEnemyWithHighestCombatGrade()
{
	float LocalHighestValue = 0.0f;
	APawn* LocalHighestGradedEnemyInstance = nullptr;

	for (auto& EnemyGrade : EnemyCombatGradesMap)
	{
		
		if (EnemyGrade.Value > LocalHighestValue)
		{
			LocalHighestValue = EnemyGrade.Value;
			LocalHighestGradedEnemyInstance = EnemyGrade.Key;
		}
	}

	return LocalHighestGradedEnemyInstance;
}

