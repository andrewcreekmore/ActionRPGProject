// © 2022 Andrew Creekmore 


#include "../World/LootableActor.h"
#include "../Components/InteractionComponent.h"
#include "../Components/InventoryComponent.h"
#include "../Items/Item.h"
#include "../World/ItemSpawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DataTable.h"

#define LOCTEXT_NAMESPACE "LootableActor"

// sets default values
ALootableActor::ALootableActor()
{
 	LootContainerMesh = CreateDefaultSubobject<USkeletalMeshComponent>("LootContainerMesh");
	SetRootComponent(LootContainerMesh);

	LootInteraction = CreateDefaultSubobject<UInteractionComponent>("LootInteraction");
	LootInteraction->InteractableActionText = LOCTEXT("LootActorText", "Loot");
	LootInteraction->InteractableNameText = LOCTEXT("LootActorName", "Chest");
	LootInteraction->SetupAttachment(GetRootComponent());

	Inventory = CreateDefaultSubobject<UInventoryComponent>("Inventory");
	Inventory->SetCapacity(20);
	Inventory->SetWeightCapacity(80.0f);


	bHasBeenLooted = false;
	LootableActorID = MakeUniqueObjectName(GetOuter(), GetClass());

	// by default, between 2 and 8 rolls
	LootRolls = FIntPoint(2, 8);
}

// called when the game starts or when spawned
void ALootableActor::BeginPlay()
{
	Super::BeginPlay();
	
	LootInteraction->OnInteract.AddDynamic(this, &ALootableActor::OnInteract);

	if (LootTable)
	{
		TArray<FLootTableRow*> SpawnItems;
		LootTable->GetAllRows("", SpawnItems);

		int32 Rolls = FMath::RandRange(LootRolls.GetMin(), LootRolls.GetMax());

		for (int32 i = 0; i < Rolls; ++i)
		{
			const FLootTableRow* LootRow = SpawnItems[FMath::RandRange(0, SpawnItems.Num() - 1)];

			ensure(LootRow);

			// generate random number
			float ProbabilityRoll = FMath::FRandRange(0.0f, 1.0f);

			// check if number is within probability range
			while (ProbabilityRoll > LootRow->Probability)
			{
				LootRow = SpawnItems[FMath::RandRange(0, SpawnItems.Num() - 1)];
				ProbabilityRoll = FMath::FRandRange(0.0f, 1.0f);
			}

			if (LootRow && LootRow->Items.Num())
			{
				// get and spawn items
				for (auto& ItemClass : LootRow->Items)
				{
					if (ItemClass)
					{
						const int32 Quantity = Cast<UItem>(ItemClass->GetDefaultObject())->GetQuantity();
						Inventory->TryAddItemFromClass(ItemClass, Quantity);
					}
				}
			}
		}
	}
}




#undef LOCTEXT_NAMESPACE