// © 2022 Andrew Creekmore 

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGPROJECT_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = "AnimationProperties")
	void UpdateAnimationProperties();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MovementDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Classes")
	class UAnimInstance* UE_AnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsAccelerating;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Classes")
	class APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Classes")
	class AMain* Main;

	UFUNCTION(BlueprintCallable)
	void EnableRootMotionMode(bool bEnable)
	{
		if (bEnable)
		{
			this->RootMotionMode = ERootMotionMode::RootMotionFromMontagesOnly;
		}
		else
		{
			this->RootMotionMode = ERootMotionMode::IgnoreRootMotion;
		}
	}
	
};
