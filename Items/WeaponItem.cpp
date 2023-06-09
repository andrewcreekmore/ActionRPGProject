// © 2022 Andrew Creekmore 


#include "../Items/WeaponItem.h"
#include "../Character/Main.h"
#include "../Character/MainPlayerController.h"

UWeaponItem::UWeaponItem()
{
	WeaponItemType = EWeaponItemType::EMS_MAX;
}


bool UWeaponItem::Equip(class AMain* Character)
{
	bool bEquipSuccessful = Super::Equip(Character);

	if (bEquipSuccessful && Character)
	{ Character->EquipWeapon(this); }

	return bEquipSuccessful;
}


bool UWeaponItem::Unequip(class AMain* Character)
{
	bool bUnequipSuccessful = Super::Unequip(Character);

	if (bUnequipSuccessful && Character)
	{ Character->UnequipWeapon(); }

	return bUnequipSuccessful;
}
