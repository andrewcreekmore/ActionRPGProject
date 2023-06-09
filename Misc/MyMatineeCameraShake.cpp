// © 2022 Andrew Creekmore 


#include "../Misc/MyMatineeCameraShake.h"

UMyMatineeCameraShake::UMyMatineeCameraShake()
{
	OscillationDuration = 0.25f;
	OscillationBlendInTime = 0.05f;
	OscillationBlendOutTime = 0.05f;

	RotOscillation.Pitch.Amplitude = FMath::RandRange(3.0f, 5.0f);
	RotOscillation.Pitch.Frequency = FMath::RandRange(15.0f, 20.0f);

	RotOscillation.Yaw.Amplitude = FMath::RandRange(3.0f, 5.0f);
	RotOscillation.Yaw.Frequency = FMath::RandRange(15.0f, 20.0f);

}
