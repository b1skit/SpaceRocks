// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PowerUp.h"
#include "PowerUpPoints.generated.h"

/**
 * 
 */
UCLASS()
class SPACEROCKS_API APowerUpPoints : public APowerUp
{
	GENERATED_BODY()
public:
	// Constructor
	APowerUpPoints();
	
	// Get the powerup's point value
	uint16 GetPointValue();

private:
	uint16 PointValue = 5000;

	
};

FORCEINLINE uint16 APowerUpPoints::GetPointValue() {
	return PointValue;
}
