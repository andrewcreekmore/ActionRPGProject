// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/Vector.h"
//#include "BehaviorTree/BlackboardComponent.h"
//#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AICombatDirector.generated.h"

UCLASS()
class ACTIONRPGPROJECT_API AAICombatDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	
	// sets default values for this actor's properties
	AAICombatDirector();

	// all enemies assigned to this Director instance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable Settings")
	TArray<APawn*> AssignedEnemyList;

	// subset of AssignedEnemyList; only present if currently have Awareness->Hostile (directly engaging player)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable Settings")
	TArray<APawn*> CurrentlyHostileEnemyList;

	// all barriers that have been manually assigned to this Director instance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable Settings")
	TArray<AActor*> AssignedBarriers;

	// map of enemies to float value combat grades
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable Settings")
	TMap<APawn*, float> EnemyCombatGradesMap;

	// map of enemies to stamps of game time in seconds, made when last they were directed to attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable Settings")
	TMap<APawn*, float> EnemyAttackRecencyMap;

	// tuning weight for position relative to camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable Settings")
	float PositionRelativeToCameraWeight;

	// tuning weight for attack recency
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable Settings")
	float AttackRecencyWeight;

	// instance-variable between-attack delay timing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable Settings")
	float DelayBetweenIssuingAttacks;

protected:
	
	// called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	// called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	TArray<float> RecalculateCombatGrading(APawn* EnemyInstance);

	UFUNCTION(BlueprintCallable)
	float GradeByPositionRelativeToCamera(APawn* EnemyInstance);

	UFUNCTION(BlueprintCallable)
	float GradeByAttackRecency(APawn* EnemyInstance);

	UFUNCTION(BlueprintCallable)
	APawn* FindEnemyWithHighestCombatGrade();
};
