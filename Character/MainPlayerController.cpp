// © 2022 Andrew Creekmore 


#include "MainPlayerController.h"
#include "../Character/Main.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AMainPlayerController::AMainPlayerController()
{

}


void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
}


void AMainPlayerController::ClientShowNotification(const FText& Message)
{
	ShowNotification(Message);
}


void AMainPlayerController::Respawn()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}


