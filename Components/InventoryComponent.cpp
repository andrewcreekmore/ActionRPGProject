// © 2022 Andrew Creekmore 


#include "../Components/InventoryComponent.h"

#define LOCTEXT_NAMESPACE "Inventory"

// sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{

}



FItemAddResult UInventoryComponent::TryAddItem(class UItem* Item)
{
	// call internal add function
	return TryAddItem_Internal(Item);
}


FItemAddResult UInventoryComponent::TryAddItemFromClass(TSubclassOf<class UItem> ItemClass, const int32 Quantity)
{
	// construct new item, set quantity, call internal add function
	UItem* Item = NewObject<UItem>(GetOwner(), ItemClass);
	Item->SetQuantity(Quantity);
	return TryAddItem_Internal(Item);
}


int32 UInventoryComponent::ConsumeItem(class UItem* Item)
{
	if (Item)
	{ ConsumeItem(Item, Item->GetQuantity()); }

	return 0;
}


int32 UInventoryComponent::ConsumeItem(class UItem* Item, const int32 Quantity)
{
	if (GetOwner() && Item)
	{
		// can't consume more than we have
		const int32 RemoveQuantity = FMath::Min(Quantity, Item->GetQuantity());

		// shouldn't have a negative quantity after consumption
		ensure(!(Item->GetQuantity() - RemoveQuantity < 0));

		// now have zero of this item; remove it from inventory
		Item->SetQuantity(Item->GetQuantity() - RemoveQuantity);

		if (Item->GetQuantity() <= 0)
		{ RemoveItem(Item); }

		else
		{ OnInventoryUpdated.Broadcast(); }

		return RemoveQuantity;
	}

	return 0;
}


// removes a single item from the inventory
bool UInventoryComponent::RemoveItem(class UItem* Item)
{
	if (GetOwner())
	{
		if (Item)
		{
			Items.RemoveSingle(Item);
			OnRep_Items();
			ReplicatedItemsKey++;

			return true;
		}
	}

	return false;
}


// returns true if we have a given amount of an item
bool UInventoryComponent::HasItem(TSubclassOf<class UItem> ItemClass, const int32 Quantity /*= 1*/) const
{
	if (UItem* ItemToFind = FindItemByClass(ItemClass))
	{ return ItemToFind->GetQuantity() >= Quantity; }

	return false;
}


// returns the first item with the same class as the given item
UItem* UInventoryComponent::FindItem(class UItem* Item) const
{
	if (Item)
	{
		for (auto& InvItem : Items)
		{
			if (InvItem && InvItem->GetClass() == Item->GetClass())
			{ return InvItem; }
		}
	}

	return nullptr;
}


// returns the first item with the same class as ItemClass
UItem* UInventoryComponent::FindItemByClass(TSubclassOf<class UItem> ItemClass) const
{
	for (auto& InvItem : Items)
	{
		if (InvItem && InvItem->GetClass() == ItemClass)
		{ return InvItem; }
	}

	return nullptr;
}


// get all inventory items that are a child of ItemClass. useful for getting all Weapons, all Consumables, etc
TArray<UItem*> UInventoryComponent::FindItemsByClass(TSubclassOf<class UItem> ItemClass) const
{
	TArray<UItem*> ItemsOfClass;

	for (auto& InvItem : Items)
	{
		if (InvItem && InvItem->GetClass()->IsChildOf(ItemClass))
		{ ItemsOfClass.Add(InvItem); }
	}

	return ItemsOfClass;
}


float UInventoryComponent::GetCurrentWeight() const
{
	float Weight = 0.0f;

	for (auto& Item : Items)
	{
		if (Item)
		{ Weight += Item->GetStackWeight(); }
	}

	return Weight;
}


void UInventoryComponent::SetWeightCapacity(const float NewWeightCapacity)
{
	WeightCapacity = NewWeightCapacity;
	OnInventoryUpdated.Broadcast();
}


void UInventoryComponent::SetCapacity(const int32 NewCapacity)
{
	Capacity = NewCapacity;
	OnInventoryUpdated.Broadcast();
}


void UInventoryComponent::OnRep_Items()
{
	OnInventoryUpdated.Broadcast();
}


UItem* UInventoryComponent::AddItem(class UItem* Item)
{
	if (GetOwner())
	{
		// reconstruct/duplicate item, this instance owned by inventory component
		UItem* NewItem = NewObject<UItem>(GetOwner(), Item->GetClass());
		NewItem->SetQuantity(Item->GetQuantity());
		NewItem->OwningInventory = this;
		NewItem->bShouldNotifyOnInventoryAdd = Item->bShouldNotifyOnInventoryAdd;
		NewItem->bShouldAutoEquip = Item->bShouldAutoEquip;
		NewItem->AddedToInventory(this, Item->GetQuantity());
		Items.Add(NewItem);
		OnRep_Items();

		return NewItem;
	}

	return nullptr;
}

// wrapper function for AddItem - checks capacity/stacks prior to add, and adds partial if needed
FItemAddResult UInventoryComponent::TryAddItem_Internal(class UItem* Item)
{
	if (GetOwner())
	{
		const int32 AddAmount = Item->GetQuantity();

		// check capacity for room; add None if full
		if (Items.Num() + 1 > GetCapacity())
		{ return FItemAddResult::AddedNone(AddAmount, LOCTEXT("InventoryCapacityFullText", "Couldn't add item to inventory; inventory is full.")); }

		// items with zero weight don't require a weight check
		if (!FMath::IsNearlyZero(Item->Weight))
		{
			// check weight capacity; add None if reached
			if (GetCurrentWeight() + Item->Weight > GetWeightCapacity())
			{ return FItemAddResult::AddedNone(AddAmount, LOCTEXT("InventoryTooMuchWeightText", "Couldn't add item to inventory; carrying too much weight.")); }
		}

		// if item is stackable, check if we already have any and if so add to corresponding stack
		if (Item->bStackable)
		{
			// should never go over max stack size
			ensure(Item->GetQuantity() <= Item->MaxStackSize);

			// if already have some of item and stackable, modify (increment) existing inventory quantity instead of adding entirely new
			if (UItem* ExistingItem = FindItem(Item))
			{
				// if room in stack
				if (ExistingItem->GetQuantity() < ExistingItem->MaxStackSize)
				{
					// determine how much of the item to add
					const int32 CapacityMaxAddAmount = ExistingItem->MaxStackSize - ExistingItem->GetQuantity();
					int32 ActualAddAmount = FMath::Min(AddAmount, CapacityMaxAddAmount);

					FText ErrorText = LOCTEXT("InventoryErrorText", "Couldn't add all of the item to your inventory.");

					// adjust based on how much weight we can carry
					if (!FMath::IsNearlyZero(Item->Weight))
					{
						// find the max amount of the item we could take on (due to weight)
						const int32 WeightMaxAddAmount = FMath::FloorToInt((WeightCapacity - GetCurrentWeight()) / Item->Weight);
						ActualAddAmount = FMath::Min(ActualAddAmount, WeightMaxAddAmount);

						if (ActualAddAmount < AddAmount)
						{ ErrorText = FText::Format(LOCTEXT("InventoryTooMuchWeightText", "Couldn't add entire stack of {ItemName} to inventory."), Item->ItemDisplayName); }
					}

					else if (ActualAddAmount < AddAmount)
					{
						// if the item weighs 0 but we can't take it, then the issue was capacity
						ErrorText = FText::Format(LOCTEXT("InventoryCapacityFullText", "Couldn't add entire stack of {ItemName} to inventory. Inventory was full."), Item->ItemDisplayName);
					}

					// we couldn't add *any* of the item to inventory
					if (ActualAddAmount <= 0)
					{ return FItemAddResult::AddedNone(AddAmount, LOCTEXT("InventoryErrorText", "Couldn't add item to inventory.")); }

					// success, checks passed: increment item quantity
					ExistingItem->SetQuantity(ExistingItem->GetQuantity() + ActualAddAmount);
					ExistingItem->bShouldNotifyOnInventoryAdd = Item->bShouldNotifyOnInventoryAdd;
					ExistingItem->bShouldAutoEquip = Item->bShouldAutoEquip;
					// call AddedToInventory for client notification / sound effect
					ExistingItem->AddedToInventory(this, ActualAddAmount);

					// if we somehow get more of the item than the max stack size, something is wrong with the math
					ensure(ExistingItem->GetQuantity() <= ExistingItem->MaxStackSize);

					if (ActualAddAmount < AddAmount)
					{ return FItemAddResult::AddedSome(AddAmount, ActualAddAmount, ErrorText); }

					else
					{ return FItemAddResult::AddedAll(AddAmount); }
				}

				else
				{ return FItemAddResult::AddedNone(AddAmount, FText::Format(LOCTEXT("InventoryFullStackText", "Couldn't add {ItemName}. You already have a full stack of this item."), Item->ItemDisplayName)); }
			}

			else
			{
				// since we do not have any of this item, add the full stack
				AddItem(Item);
				return FItemAddResult::AddedAll(AddAmount);
			}
		}

		else // item isn't stackable
		{
			// non-stackable items should always have a quantity of 1
			ensure(Item->GetQuantity() == 1);

			AddItem(Item);
			return FItemAddResult::AddedAll(AddAmount);
		}
	}

	return FItemAddResult::AddedNone(-1, LOCTEXT("ErrorMessage", ""));
}

#undef LOCTEXT_NAMESPACE