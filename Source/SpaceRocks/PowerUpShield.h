// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PowerUp.h"
#include "PowerUpShield.generated.h"

/**
 * 
 */
UCLASS()
class SPACEROCKS_API APowerUpShield : public APowerUp
{
	GENERATED_BODY()
public:
	// Constructor
	APowerUpShield();

	// Get the value of the powerup
	uint8 GetPowerUpValue();
	
private:
	float ShieldPowerUpValue = 50; // Value to increase the shield by
};
