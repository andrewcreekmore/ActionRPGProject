// © 2022 Andrew Creekmore 


#include "ActionRPGProject/Components/InteractionComponent.h"
#include "../Character/Main.h"

UInteractionComponent::UInteractionComponent()
{	
	// no need for tick
	SetComponentTickEnabled(false);

	InteractionTime = 0.f;
	InteractionDistance = 200.f;
	bHideInteractCardOnInteract = true;
	bHideOutlineOnInteract = true;
	bHideOutlineOnEndFocus = true;
	InteractableNameText = FText::FromString("Interactable Object");
	InteractableActionText = FText::FromString("Interact");

	Space = EWidgetSpace::Screen;
	DrawSize = FIntPoint(400, 100);
	bDrawAtDesiredSize = true;

	// active but hidden by default
	SetActive(true);
	SetHiddenInGame(true);
}


void UInteractionComponent::Deactivate()
{
	Super::Deactivate();

	for (int32 i = Interactors.Num() - 1; i >= 0; --i)
	{
		if (AMain* Interactor = Interactors[i])
		{
			EndFocus(Interactor);
			EndInteract(Interactor);
		}
	}

	Interactors.Empty();
}


bool UInteractionComponent::CanInteract(class AMain* Character) const
{
	const bool bPlayerAlreadyInteracting = Interactors.Num() >= 1;
	return !bPlayerAlreadyInteracting && IsActive() && GetOwner() != nullptr && Character != nullptr;
}


void UInteractionComponent::RefreshWidget()
{
	if (!bHiddenInGame)
	{
		if (UInteractionWidget* InteractionWidget = Cast<UInteractionWidget>(GetUserWidgetObject()))
		{ InteractionWidget->UpdateInteractionWidget(this); }
	}
}


void UInteractionComponent::BeginFocus(class AMain* Character)
{
	if (!IsActive() || !GetOwner() || !Character)
	{ return; }

	// call delegate
	OnBeginFocus.Broadcast(Character);

	// show UI interaction card widget
	SetHiddenInGame(false);
	
	// show outline around object
	TArray<UActorComponent*> Components;
	GetOwner()->GetComponents(UPrimitiveComponent::StaticClass(), Components);

	for (auto& VisualComp : Components)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
		{ Prim->SetRenderCustomDepth(true); }
	}

	RefreshWidget();
}


void UInteractionComponent::EndFocus(class AMain* Character)
{
	// call delegate
	OnEndFocus.Broadcast(Character);

	// hide UI interaction card widget
	SetHiddenInGame(true);

	// hide outline around object
	if (bHideOutlineOnEndFocus)
	{
		TArray<UActorComponent*> Components;
		GetOwner()->GetComponents(UPrimitiveComponent::StaticClass(), Components);

		for (auto& VisualComp : Components)
		{
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
			{ Prim->SetRenderCustomDepth(false); }
		}
	}
}


void UInteractionComponent::BeginInteract(class AMain* Character)
{
	Interactors.AddUnique(Character);
	OnBeginInteract.Broadcast(Character);

	// hide UI interaction card widget (can be controlled in blueprint case by case)
	if (bHideInteractCardOnInteract)
	{ SetHiddenInGame(true); }

	// hide item outline on interact (so items not immediately picked up, e.g. chests, don't stay outlined past interact point)
	if (bHideOutlineOnInteract)
	{
		TArray<UActorComponent*> Components;
		GetOwner()->GetComponents(UPrimitiveComponent::StaticClass(), Components);

		for (auto& VisualComp : Components)
		{
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
			{ Prim->SetRenderCustomDepth(false); }
		}
	}
}


void UInteractionComponent::EndInteract(class AMain* Character)
{
	Interactors.RemoveSingle(Character);
	OnEndInteract.Broadcast(Character);
}


void UInteractionComponent::Interact(class AMain* Character)
{
	OnInteract.Broadcast(Character);
}


float UInteractionComponent::GetInteractPercentage()
{
	if (Interactors.IsValidIndex(0))
	{
		if (AMain* Interactor = Interactors[0])
		{
			if (Interactor && Interactor->IsInteracting())
			{
				return 1.0f - FMath::Abs(Interactor->GetRemainingInteractTime() / InteractionTime);
			}
		}
	}

	return 0.0f;
}


void UInteractionComponent::SetInteractableNameText(const FText& NewNameText)
{
	InteractableNameText = NewNameText;
	RefreshWidget();
}


void UInteractionComponent::SetInteractableActionText(const FText& NewActionText)
{
	InteractableActionText = NewActionText;
	RefreshWidget();
}
