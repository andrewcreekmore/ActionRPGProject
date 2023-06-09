// © 2022 Andrew Creekmore 


#include "../Items/EquippableItem.h"
#include "../Character/Main.h"
#include "../Character/MainPlayerController.h"
#include "../Components/InventoryComponent.h"

#define LOCTEXT_NAMESPACE "EquippableItem"


UEquippableItem::UEquippableItem()
{
	bStackable = false;
	bEquipped = false;
	UseActionText = LOCTEXT("ItemUseActionText", "Equip");
}


void UEquippableItem::Use(class AMain* Character)
{
	if (Character)
	{
		for (int32 i = 0; i < Slots.Num(); ++i)
		{
			// if item is already equipped, unequip. if not, equip
			if (Character->GetEquippedItems().Contains(Slots[i]) && !bEquipped)
			{
				UEquippableItem* AlreadyEquippedItem = *Character->GetEquippedItems().Find(Slots[i]);
				AlreadyEquippedItem->SetEquipped(false);
			}
		}

		SetEquipped(!IsEquipped());
	}
}


bool UEquippableItem::Equip(class AMain* Character)
{
	if (Character)
	{ return Character->EquipItem(this); }

	return false;
}


bool UEquippableItem::Unequip(class AMain* Character)
{
	if (Character)
	{ return Character->UnequipItem(this); }

	return false;
}


bool UEquippableItem::ShouldShowInInventory() const
{
	return !bEquipped;
}


void UEquippableItem::AddedToInventory(class UInventoryComponent* Inventory, int32 QuantityAdded = 1)
{
	Super::AddedToInventory(Inventory, QuantityAdded);

	if (AMain* Character = Cast<AMain>(Inventory->GetOwner()))
	{
		// only auto-equip if NOT looting from a chest / menu (i.e., singular in-world item pickups) (and flagged, see below)
		if (Character && !Character->IsLooting())
		{
			/* if we add an equippable item to inventory which is flagged as bShouldAutoEquip (i.e., is being added on load from save),
			and don't currently have any item equipped in its corresponding slot, auto-equip this item */
			if (Super::ShouldAutoEquip())
			{
				for (int32 i = 0; i < Slots.Num(); ++i)
				{
					if (!Character->GetEquippedItems().Contains(Slots[i]))
					{ SetEquipped(true); }
				}
			}
		}
	}
}


void UEquippableItem::SetEquipped(bool bNewEquipped)
{
	bEquipped = bNewEquipped;
	EquipStatusChanged();
}


void UEquippableItem::EquipStatusChanged()
{
	if (AMain* Character = Cast<AMain>(GetOuter()))
	{
		UseActionText = bEquipped ? LOCTEXT("UnequipText", "Unequip") : LOCTEXT("EquipText", "Equip");

		if (bEquipped)
		{ Equip(Character); }
		
		else
		{ Unequip(Character); }
	}

	// tell UI to update
	OnItemModified.Broadcast();
}

#undef LOCTEXT_NAMESPACE