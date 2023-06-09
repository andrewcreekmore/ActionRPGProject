// © 2022 Andrew Creekmore 


#include "../Items/GearItem.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/Main.h"

UGearItem::UGearItem()
{
	PhysicalDamageDefenseMultiplier = 0.0f;
	GearItemType = EGearItemType::EMS_MAX;

	EquipSoundVolumeMultiplier = 1.f;
	UnequipSoundVolumeMultiplier = 1.f;
}


bool UGearItem::Equip(class AMain* Character)
{
	bool bEquipSuccessful = Super::Equip(Character);

	if (bEquipSuccessful && Character)
	{ Character->EquipGear(this); }

	Character->bIsEquipping = false;

	return bEquipSuccessful;
}


bool UGearItem::Unequip(class AMain* Character)
{
	bool bUnequipSuccessful = Super::Unequip(Character);

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (bUnequipSuccessful && Character)
		{ Character->UnequipGear(Slots[i], this); }
	}

	return bUnequipSuccessful;
}
