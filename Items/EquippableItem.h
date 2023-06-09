// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "../Items/Item.h"
#include "EquippableItem.generated.h"

// equippable item slots
UENUM(BlueprintType)
enum class EEquippableSlot : uint8 
{
	EIS_Head UMETA(DisplayName = "Head"),
	EIS_Hair UMETA(DisplayName = "Hair"),
	EIS_Helmet UMETA(DisplayName = "Helmet"),
	EIS_Chest UMETA(DisplayName = "Chest"),
	EIS_Cloak UMETA(DisplayName = "Cloak"),
	EIS_Arm_R UMETA(DisplayName = "Arm_R"),
	EIS_Arm_L UMETA(DisplayName = "Arm_L"),
	EIS_Legs UMETA(DisplayName = "Legs"),
	EIS_Shoes UMETA(DisplayName = "Shoes"),
	EIS_Accessory UMETA(DisplayName = "Accessory"),
	EIS_PrimaryWeapon UMETA(DisplayName = "PrimaryWeapon"),
	EIS_SecondaryWeapon UMETA(DisplayName = "SecondaryWeapon"),
	EIS_QuickUse1 UMETA(DisplayName = "QuickUse1"),
	EIS_QuickUse2 UMETA(DisplayName = "QuickUse2"),
	EIS_QuickUse3 UMETA(DisplayName = "QuickUse3")
};


/**
 * 
 */
UCLASS(Abstract, NotBlueprintable)
class ACTIONRPGPROJECT_API UEquippableItem : public UItem
{
	GENERATED_BODY()
	
public:

	UEquippableItem();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equippables")

	TArray<EEquippableSlot> Slots;

	virtual void Use(class AMain* Character) override;

	UFUNCTION(BlueprintCallable, Category = "Equippables")
	virtual bool Equip(class AMain* Character);

	UFUNCTION(BlueprintCallable, Category = "Equippables")
	virtual bool Unequip(class AMain* Character);

	virtual bool ShouldShowInInventory() const override;
	virtual void AddedToInventory(class UInventoryComponent* Inventory, int32 QuantityAdded) override;

	UFUNCTION(BlueprintPure, Category = "Equippables")
	bool IsEquipped() { return bEquipped; };

	void SetEquipped(bool bNewEquipped);


protected:

	UPROPERTY()
	bool bEquipped;

	UFUNCTION()
	void EquipStatusChanged();

};
