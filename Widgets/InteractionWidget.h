// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

class UInteractionComponent;

/**
 * 
 */
UCLASS()
class ACTIONRPGPROJECT_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void UpdateInteractionWidget(class UInteractionComponent* InteractionComponent);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateInteractionWidget();

	UPROPERTY(BlueprintReadOnly, Category = "Interaction", meta = (ExposeOnSpawn))
	class UInteractionComponent* OwningInteractionComponent;
};
