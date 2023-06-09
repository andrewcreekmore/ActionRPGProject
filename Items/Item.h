// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

class UAudioComponent;
class USoundCue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemModified);

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	IR_Common UMETA(DisplayName = "Common"),
	IR_Uncommon UMETA(DisplayName = "Uncommon"),
	IR_Rare UMETA(DisplayName = "Rare"),
	IR_Legendary UMETA(DisplayName = "Legendary")
};

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class ACTIONRPGPROJECT_API UItem : public UObject
{
	GENERATED_BODY()

protected:

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	
	UItem();

	// mesh to display for this item's in-world pickup
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	class UStaticMesh* PickupMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bShouldNotifyOnInventoryAdd;

	UFUNCTION(Category = "Item")
	FORCEINLINE bool ShouldNotifyOnAdd() const { return bShouldNotifyOnInventoryAdd; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bShouldAutoEquip;

	UFUNCTION(Category = "Item")
	FORCEINLINE bool ShouldAutoEquip() const { return bShouldAutoEquip; }

	// on-pickup sound
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	USoundBase* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bShouldPlayPickupSound;

	// thumbnail inventory picture for this item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	class UTexture2D* Thumbnail;

	// inventory display name for this item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FText ItemDisplayName;

	// an optional description of the item to display in the inventory
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (MultiLine = true))
	FText ItemDescription;

	// the verb text for using the item (e.g., equip, eat, etc)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FText UseActionText;

	// rarity of the item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	EItemRarity Rarity;

	// weight of the item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (ClampMin = 0.0))
	float Weight;

	// whether or not this item can be stacked in the inventory
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	bool bStackable;

	// maximum size a stack of these items can be
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (ClampMin = 2, EditCondition = bStackable))
	int32 MaxStackSize;

	// inventory tooltip for this item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	TSubclassOf<class UItemTooltip> ItemTooltip;

	// amount of the item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (UIMin = 1, EditCondition = bStackable))
	int32 Quantity;

	// the inventory that owns this item
	UPROPERTY()
	class UInventoryComponent* OwningInventory;

	UPROPERTY(BlueprintAssignable)
	FOnItemModified OnItemModified;


public:

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetQuantity(const int32 NewQuantity);

	UFUNCTION(BlueprintCallable, Category = "Item")
	FORCEINLINE int32 GetQuantity() const { return Quantity; }
	
	UFUNCTION(BlueprintCallable, Category = "Item")
	FORCEINLINE float GetStackWeight() const { return Quantity * Weight; }

	UFUNCTION(BlueprintPure, Category = "Item")
	virtual bool ShouldShowInInventory() const;

	virtual void Use(class AMain* Character);

	UFUNCTION(BlueprintCallable)
	virtual void AddedToInventory(class UInventoryComponent* Inventory, int32 QuantityAdded);

};
