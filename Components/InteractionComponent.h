// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "../Widgets/InteractionWidget.h"
#include "InteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginInteract, class AMain*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndInteract, class AMain*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginFocus, class AMain*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndFocus, class AMain*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, class AMain*, Character);

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACTIONRPGPROJECT_API UInteractionComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	// set default parameters
	UInteractionComponent();

	// the time the player must hold the interact key to interact with this object
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionTime;

	// the max distance the player can be away from this actor and still interact with it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionDistance;

	// the name that will show in the UI when the player looks at the interactable
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractableNameText;

	// the verb that describes how the interaction works, e.g., "light" for a fireplace, "eat" for food, etc
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractableActionText;

	// whether or not to hide the interaction prompt card when the interaction begins (on by default; off for things like switches)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bHideInteractCardOnInteract;

	// whether or not to hide the item outline effect when the interaction begins (on by default)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bHideOutlineOnInteract;

	// whether or not to hide the item outline effect when the interaction begins (on by default)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bHideOutlineOnEndFocus;

	// DELEGATES

	// called when the player presses the interact key while focusing on this interactable actor
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginInteract OnBeginInteract;

	// called when the player releases the interact key, stops looking at the interactable actor, or gets too far away after starting an interact
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnEndInteract OnEndInteract;

	// called when the player presses the interact key while focusing on this interactable actor
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginFocus OnBeginFocus;

	// called when the player releases the interact key, stops looking at the interactable actor, or gets too far away after starting an interact
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnEndFocus OnEndFocus;

	// called when the player has interacted with the item for the required amount of time
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginInteract OnInteract;

protected:
	
	// called when the game starts
	virtual void Deactivate() override;

	bool CanInteract(class AMain* Character) const;

	// holds player characters able to interact
	UPROPERTY()
	TArray<class AMain*> Interactors;

public:

	// refresh the interaction UI widget and its custom widgets (e.g., to update a displayed quantity, etc)
	void RefreshWidget();

	// called when the player's interaction check trace begins/ends hitting this item
	void BeginFocus(class AMain* Character);
	void EndFocus(class AMain* Character);

	// called when the player begins/ends interaction with the item
	void BeginInteract(class AMain* Character);
	void EndInteract(class AMain* Character);

	void Interact(class AMain* Character);

	// returns a value from 0-1 denoting how far through the interaction process we are
	UFUNCTION(BlueprintPure, Category = "Interaction")
	float GetInteractPercentage();

	// call this to change the name of the interactable; will also refresh the interaction UI widget
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractableNameText(const FText& NewNameText);

	// call this to change the verb of the interactable; will also refresh the interaction UI widget
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractableActionText(const FText& NewActionText);
};
