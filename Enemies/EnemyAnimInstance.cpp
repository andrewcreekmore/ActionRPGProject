// © 2022 Andrew Creekmore 


#include "EnemyAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemy.h"


void UEnemyAnimInstance::NativeInitializeAnimation()
{
	// get reference to enemy pawn
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{ Enemy = Cast<AEnemy>(Pawn); }
	}
}

void UEnemyAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{ Enemy = Cast<AEnemy>(Pawn); }
	}

	if (Pawn)
	{
		// get reference to enemy pawn's movement speed
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		// get magnitude of LateralSpeed vector as a float and assign to MovementSpeed
		MovementSpeed = LateralSpeed.Size();

		MovementDirection = (UE_AnimInstance->CalculateDirection(Speed, Pawn->GetActorRotation()));

		if (Enemy->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{ bIsAccelerating = true; }

		else
		{ bIsAccelerating = false; }

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		if (Enemy == nullptr)
		{ Enemy = Cast<AEnemy>(Pawn); }
	}
}

