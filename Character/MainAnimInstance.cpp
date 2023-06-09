// © 2022 Andrew Creekmore 


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Main.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{ Main = Cast<AMain>(Pawn); }
	}
}

void UMainAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{ Pawn = TryGetPawnOwner(); }

	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		// get magnitude of LateralSpeed vector as a float and assign to MovementSpeed
		MovementSpeed = LateralSpeed.Size();

		MovementDirection = (UE_AnimInstance->CalculateDirection(Speed, Pawn->GetActorRotation()));

		if (Main->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{ bIsAccelerating = true; }

		else
		{ bIsAccelerating = false; }

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		if (Main == nullptr)
		{ Main = Cast<AMain>(Pawn); }
	}
}

