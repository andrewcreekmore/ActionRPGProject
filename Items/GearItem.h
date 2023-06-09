// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "../Items/EquippableItem.h"
#include "GearItem.generated.h"

UENUM(BlueprintType)
enum class EGearItemType : uint8
{
	EMS_LeatherGloves UMETA(DisplayName = "LeatherGloves"),
	EMS_LeatherBoots UMETA(DisplayName = "LeatherBoots"),
	EMS_LeatherArmor UMETA(DisplayName = "LeatherArmor"),
	EMS_SquireArmor UMETA(DisplayName = "SquireArmor"),
	EMS_KnightArmor UMETA(DisplayName = "KnightArmor"),
	EMS_KnightGauntlets UMETA(DisplayName = "KnightGauntlets"),
	EMS_KnightGreaves UMETA(DisplayName = "KnightGreaves"),
	EMS_HeavyKnightArmor UMETA(DisplayName = "HeavyKnightArmor"),
	EMS_HeavyKnightGreaves UMETA(DisplayName = "HeavyKnightGreaves"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

/**
 * 
 */
UCLASS(Blueprintable)
class ACTIONRPGPROJECT_API UGearItem : public UEquippableItem
{
	GENERATED_BODY()

public:

	UGearItem();

	virtual bool Equip(class AMain* Character) override;
	virtual bool Unequip(class AMain* Character) override;

	// the skeletal mesh corresponding to this gear
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gear")
	//class USkeletalMesh* Mesh;
	TArray<class USkeletalMesh*> Meshes;

	// optional material instance to apply to the gear
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gear")
	class UMaterialInstance* MaterialInstance;

	// amount of physical defense this item provides. 0.2 = 20% less physical damage taken
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gear", meta = (ClampMin = 0.0, ClampMax = 1.0))
	float PhysicalDamageDefenseMultiplier;

	// gear data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gear")
	EGearItemType GearItemType;

	// on-equip sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float EquipSoundVolumeMultiplier;

	// on-unequip/sheath sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* UnequipSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float UnequipSoundVolumeMultiplier;
	
};
