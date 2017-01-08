// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Saucer.h"
#include "SmallSaucer.generated.h"

/**
 * 
 */
UCLASS()
class SPACEROCKS_API ASmallSaucer : public ASaucer
{
	GENERATED_BODY()
public:
	// Constructor
	ASmallSaucer();
	
private:
	uint16 PointValue = 1000;

	// Saucer weapons
	FVector WeaponOffset = FVector(120.0f, 0.0f, 0.0f);
	FTimerHandle TimerHandle_ShotTimerExpired;	// Handle for efficient management of ShotTimerExpired timer
	const float FIRE_RATE = 0.75f;				// How fast the weapon will fire
	virtual void FireShot() override;
	const float MAX_FIRING_JITTER = 31.0f;		// +/- Degrees of jitter to start with at level 1 (total jitter angle *= 2).
};
