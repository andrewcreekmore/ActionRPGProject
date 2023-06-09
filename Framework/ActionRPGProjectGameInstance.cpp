// © 2022 Andrew Creekmore 


#include "ActionRPGProject/Framework/ActionRPGProjectGameInstance.h"
#include <MoviePlayer/Public/MoviePlayer.h>

UActionRPGProjectGameInstance::UActionRPGProjectGameInstance()
{
	// set default values
	BaseItemID = 0;
}


void UActionRPGProjectGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UActionRPGProjectGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UActionRPGProjectGameInstance::EndLoadingScreen);
}

void UActionRPGProjectGameInstance::BeginLoadingScreen(const FString& InMapName)
{
	if (!IsRunningDedicatedServer())
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}
}

void UActionRPGProjectGameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{

}