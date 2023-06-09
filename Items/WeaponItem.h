// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "../Items/EquippableItem.h"
#include "WeaponItem.generated.h"

UENUM(BlueprintType)
enum class EWeaponItemType : uint8
{
	EMS_RustedSword UMETA(DisplayName = "RustedSword"),
	EMS_Broadsword UMETA(DisplayName = "Broadsword"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

/**
 * 
 */
UCLASS(Blueprintable)
class ACTIONRPGPROJECT_API UWeaponItem : public UEquippableItem
{
	GENERATED_BODY()

public:

	UWeaponItem();

	virtual bool Equip(class AMain* Character) override;
	virtual bool Unequip(class AMain* Character) override;

	// the weapon actor class to give to the player upon equipping this weapon item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> WeaponClass;

	// weapon data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponItemType WeaponItemType;
	
};
