// © 2022 Andrew Creekmore 


#include "../Items/ShieldItem.h"
#include "../Character/Main.h"
#include "../Character/MainPlayerController.h"

UShieldItem::UShieldItem()
{
	ShieldItemType = EShieldItemType::EMS_MAX;
}


bool UShieldItem::Equip(class AMain* Character)
{
	bool bEquipSuccessful = Super::Equip(Character);

	if (bEquipSuccessful && Character)
	{ Character->EquipShield(this); }

	return bEquipSuccessful;
}


bool UShieldItem::Unequip(class AMain* Character)
{
	bool bUnequipSuccessful = Super::Unequip(Character);

	if (bUnequipSuccessful && Character)
	{ Character->UnequipShield(); }

	return bUnequipSuccessful;
}
