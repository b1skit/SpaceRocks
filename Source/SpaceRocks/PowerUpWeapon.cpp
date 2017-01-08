// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceRocks.h"
#include "PowerUpWeapon.h"

// Constructor
APowerUpWeapon::APowerUpWeapon() {
	// Set PowerUp material:
	static ConstructorHelpers::FObjectFinder<UMaterial>PowerUpMaterialInstance(TEXT("Material'/Game/Art/Materials/DEBUG_MATERIAL.DEBUG_MATERIAL'")); // Get a reference to an asset in the content browser
	if (PowerUpMaterialInstance.Succeeded())
		Super::PowerUpMaterial = (UMaterial*)PowerUpMaterialInstance.Object;		// Set the material
}


