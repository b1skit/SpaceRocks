// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Asteroid.h"
#include "AsteroidMedium.generated.h"

/**
 * 
 */
UCLASS()
class SPACEROCKS_API AAsteroidMedium : public AAsteroid
{
	GENERATED_BODY()
	
	// Constructor
	AAsteroidMedium();

	//// Generates a random starting speed for an Asteroid
	//// Returns a float between min and max Asteroid speed
	//float GetStartingSpeed();

	float MediumMeshScaleFactor = 0.75f;
	
};
