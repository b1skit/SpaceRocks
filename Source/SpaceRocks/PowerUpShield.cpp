// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceRocks.h"
#include "PowerUpShield.h"

APowerUpShield::APowerUpShield() {
	// Set PowerUp material:
	static ConstructorHelpers::FObjectFinder<UMaterial>PowerUpMaterialInstance(TEXT("Material'/Game/Art/Materials/PowerUpShieldMaterial.PowerUpShieldMaterial'")); // Get a reference to an asset in the content browser
	if (PowerUpMaterialInstance.Succeeded())
		Super::PowerUpMaterial = (UMaterial*)PowerUpMaterialInstance.Object;		// Set the material

}

// Get the value of the powerup
uint8 APowerUpShield::GetPowerUpValue() {
	return ShieldPowerUpValue;
}

