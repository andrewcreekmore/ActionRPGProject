// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Character/Main.h"
#include "../Components/InteractionComponent.h"
#include "../Components/InventoryComponent.h"
#include "../Items/Item.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/ActorChannel.h"
#include "Pickup.generated.h"

UCLASS()
class ACTIONRPGPROJECT_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	
	// sets default values for this actor's properties
	APickup();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName PickupID;

	// takes the item to represent and creates the pickup from it. performed on BeginPlay and when a player drops an item on the ground
	void InitializePickup(const TSubclassOf<class UItem> ItemClass, const int32 Quantity);

	// aligns pickup's rotation with ground's rotation
	UFUNCTION(BlueprintImplementableEvent)
	void AlignWithGround();

	// used as a template to create the pickup when spawned in
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	class UItem* ItemTemplate;

protected:
	
	// called when the game starts or when spawned
	virtual void BeginPlay() override;

	// the item that will be added to the inventory when this pickup is taken
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UItem* Item;

	UFUNCTION()
	void OnRep_Item();

	// if some property of the item is modified, we bind this to OnItemModified and refresh the UI
	UFUNCTION()
	void OnItemModified();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// called when player takes the pickup
	UFUNCTION()
	void OnTakePickup(class AMain* Taker);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UInteractionComponent* InteractionComponent;
};