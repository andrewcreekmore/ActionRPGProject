// © 2022 Andrew Creekmore 


#include "../Widgets/InteractionWidget.h"

void UInteractionWidget::UpdateInteractionWidget(class UInteractionComponent* InteractionComponent)
{
	OwningInteractionComponent = InteractionComponent;
	OnUpdateInteractionWidget();
}
