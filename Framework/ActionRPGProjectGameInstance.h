// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ActionRPGProjectGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGPROJECT_API UActionRPGProjectGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UActionRPGProjectGameInstance();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 BaseItemID;

	virtual void Init() override;

	UFUNCTION()
	virtual void BeginLoadingScreen(const FString& MapName);
	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* InLoadedWorld);
	
};
