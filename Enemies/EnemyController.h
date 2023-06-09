// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGPROJECT_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:

	AEnemyController();

	// called when the game starts or when spawned
	virtual void BeginPlay() override;

	// called when the AIController is taken over
	virtual void OnPossess(APawn* Pawn) override;

private:

	// blackboard component for this enemy
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	class UBlackboardComponent* BlackboardComponent;

	// behavior tree component for this enemy
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* BehaviorTreeComponent;
	
public:

	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const { return BlackboardComponent; }


};
