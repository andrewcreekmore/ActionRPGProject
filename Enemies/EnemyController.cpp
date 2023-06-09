// © 2022 Andrew Creekmore 


#include "EnemyController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionComponent.h"
#include "Enemy.h"


AEnemyController::AEnemyController()
{	
	// construct blackboard component
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	// assert valid; halt execution if not
	check(BlackboardComponent);

	// construct behavior tree component
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	// assert valid; halt execution if not
	check(BlackboardComponent);
}


// called when the game starts or when spawned
void AEnemyController::BeginPlay()
{
	Super::BeginPlay();
}


void AEnemyController::OnPossess(APawn* InPawn)
{
	// call to parent function
	Super::OnPossess(InPawn);

	if (InPawn == nullptr) { return; }
	
	AEnemy* Enemy = Cast<AEnemy>(InPawn);

	// initialize blackboard component
	if (Enemy)
	{
		if (Enemy->GetBehaviorTree())
		{ BlackboardComponent->InitializeBlackboard(*(Enemy->GetBehaviorTree()->BlackboardAsset)); }
	}
}