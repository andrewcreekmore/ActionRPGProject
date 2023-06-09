// © 2022 Andrew Creekmore 


#include "../World/Pickup.h"

// sets default values
APickup::APickup()
{
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	SetRootComponent(PickupMesh);

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>("PickupInteractionComponent");
	InteractionComponent->InteractionTime = 0.5f;
	InteractionComponent->InteractionDistance = 200.0f;
	InteractionComponent->InteractableNameText = FText::FromString("Pickup");
	InteractionComponent->InteractableActionText = FText::FromString("Take");
	InteractionComponent->OnInteract.AddDynamic(this, &APickup::OnTakePickup);
	InteractionComponent->SetupAttachment(PickupMesh);

	PickupID = MakeUniqueObjectName(GetOuter(), GetClass());
}


void APickup::InitializePickup(const TSubclassOf<class UItem> ItemClass, const int32 Quantity)
{
	if (ItemClass && Quantity > 0)
	{
		Item = NewObject<UItem>(this, ItemClass);
		Item->SetQuantity(Quantity);

		OnRep_Item();
	}
}


// called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

	if (ItemTemplate && bNetStartup)
	{ InitializePickup(ItemTemplate->GetClass(), ItemTemplate->GetQuantity()); }

	// if pickup was spawned at runtime, ensure it matches the rotate of ground it was dropped on
	if (!bNetStartup)
	{ AlignWithGround(); }
}


void APickup::OnRep_Item()
{
	if (Item)
	{
		PickupMesh->SetStaticMesh(Item->PickupMesh);
		InteractionComponent->InteractableNameText = Item->ItemDisplayName;

		// bind to this delegate in order to refresh the interaction widget if item quantity changes
		Item->OnItemModified.AddDynamic(this, &APickup::OnItemModified);
	}

	// if any properties of the item are changed, refresh the widget
	InteractionComponent->RefreshWidget();
}


void APickup::OnItemModified()
{
	if (InteractionComponent)
	{ InteractionComponent->RefreshWidget(); }
}


#if WITH_EDITOR
void APickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// if a new pickup is selected in the property editor, change the mesh to reflect the new item being selected
	if (PropertyName == GET_MEMBER_NAME_CHECKED(APickup, ItemTemplate))
	{
		if (ItemTemplate)
		{ PickupMesh->SetStaticMesh(ItemTemplate->PickupMesh); }
	}
}
#endif


void APickup::OnTakePickup(class AMain* Taker)
{
	if (!Taker)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pickup was taken but player was not valid."));
		return;
	}

	if (Item)
	{
		if (UInventoryComponent* PlayerInventory = Taker->PlayerInventory)
		{
			const FItemAddResult AddResult = PlayerInventory->TryAddItem(Item);

			if (AddResult.ActualAmountGiven < Item->GetQuantity())
			{ Item->SetQuantity(Item->GetQuantity() - AddResult.ActualAmountGiven); }

			else if (AddResult.ActualAmountGiven >= Item->GetQuantity())
			{ Destroy(); }

			// record pickup ID so we know whether to spawn pickup in world on save game load
			PlayerInventory->PickupsTaken.Add(PickupID);
		}
	}
}