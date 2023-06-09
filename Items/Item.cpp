// © 2022 Andrew Creekmore 


#include "../Items/Item.h"
#include "../Character/Main.h"
#include "../Character/MainPlayerController.h"
#include "../Components/InventoryComponent.h"
#include "../Framework/ActionRPGProjectGameInstance.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "Item"

#if WITH_EDITOR
void UItem::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// UPROPERTY clamping doesn't support using a variable to clamp, so doing it here instead (if Quantity is what changed)
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UItem, Quantity))
	{ Quantity = FMath::Clamp(Quantity, 1, bStackable ? MaxStackSize : 1); }
}
#endif

UItem::UItem()
{
	// set default values
	ItemDisplayName = LOCTEXT("ItemName", "Item");
	UseActionText = LOCTEXT("ItemUseActionText", "Use");
	Weight = 0.0f;
	bStackable = true;
	Quantity = 1;
	MaxStackSize = 2;
	bShouldNotifyOnInventoryAdd = true;
	bShouldPlayPickupSound = true;
	bShouldAutoEquip = false;
}


void UItem::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{
		Quantity = FMath::Clamp(NewQuantity, 0, bStackable? MaxStackSize : 1);
		OnItemModified.Broadcast();
	}
}

bool UItem::ShouldShowInInventory() const
{
	// by default, true
	return true;
}

void UItem::Use(class AMain* Character)
{

}

void UItem::AddedToInventory(class UInventoryComponent* Inventory, int32 QuantityAdded = 1)
{
	if (AMain* Character = Cast<AMain>(Inventory->GetOwner()))
	{
		if (bShouldNotifyOnInventoryAdd)
		{
			if (Character)
			{
				// show hud notification
				if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Character->GetController()))
				{
					FText AddedToInventoryText;

					if (QuantityAdded > 1)
					{ AddedToInventoryText = FText::Format(LOCTEXT("AddedToInventoryText", "{Quantity}x {ItemDisplayName}"), QuantityAdded, ItemDisplayName); }

					else
					{ AddedToInventoryText = FText::Format(LOCTEXT("AddedToInventoryText", "{ItemDisplayName}"), ItemDisplayName); }
					
					PlayerController->ClientShowNotification(AddedToInventoryText);
				}

				// play pickup sound
				if (PickupSound && bShouldPlayPickupSound)
				{ UGameplayStatics::PlaySound2D(GetWorld(), PickupSound); }
			}
		}
	}
}


#undef LOCTEXT_NAMESPACE