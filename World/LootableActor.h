// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LootableActor.generated.h"

UCLASS()
class ACTIONRPGPROJECT_API ALootableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	// sets default values for this actor's properties
	ALootableActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName LootableActorID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasBeenLooted;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* LootContainerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UInteractionComponent* LootInteraction;

	// the items in the lootable actor are held in this inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UInventoryComponent* Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UDataTable* LootTable;

	// the number of times to roll the loot table (random num between min and max will be used)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	FIntPoint LootRolls;


protected:
	
	// called when the game starts or when spawned
	virtual void BeginPlay() override;

};
