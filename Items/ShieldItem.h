// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "../Items/EquippableItem.h"
#include "ShieldItem.generated.h"


UENUM(BlueprintType)
enum class EShieldItemType : uint8
{
	EMS_WoodenShield UMETA(DisplayName = "WoodenShield"),
	EMS_KiteShield UMETA(DisplayName = "KiteShield"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

/**
 * 
 */
UCLASS(Blueprintable)
class ACTIONRPGPROJECT_API UShieldItem : public UEquippableItem
{
	GENERATED_BODY()

public:
	
	UShieldItem();

	virtual bool Equip(class AMain* Character) override;
	virtual bool Unequip(class AMain* Character) override;

	// the shield actor class to give to the player upon equipping this shield item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shield")
	TSubclassOf<class AShield> ShieldClass;

	// shield data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	EShieldItemType ShieldItemType;
	
};
