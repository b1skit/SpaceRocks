// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Asteroid.h"
#include "AsteroidSmall.generated.h"

/**
 * 
 */
UCLASS()
class SPACEROCKS_API AAsteroidSmall : public AAsteroid
{
	GENERATED_BODY()
	
	// Constructor
	AAsteroidSmall();

	//// Generates a random starting speed for an Asteroid
	//// Returns a float between min and max Asteroid speed
	//float GetStartingSpeed();
	
	float SmallMeshScaleFactor = 0.55f;

	//const float MIN_SMALL_ASTEROID_SPEED = 600.0f;		// The minimum speed a SMALL Asteroid could possibly go
	//const float MAX_SMALL_ASTEROID_SPEED = 900.0f;		// The maximum speed a SMALL Asteroid could possibly go
};
