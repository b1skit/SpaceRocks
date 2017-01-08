// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceRocks.h"
#include "PowerUpPoints.h"

// Constructor
APowerUpPoints::APowerUpPoints() {
	// Set PowerUp material:
	static ConstructorHelpers::FObjectFinder<UMaterial>PowerUpMaterialInstance(TEXT("Material'/Game/Art/Materials/PowerUpPointsMaterial.PowerUpPointsMaterial'")); // Get a reference to an asset in the content browser
	if (PowerUpMaterialInstance.Succeeded())
		Super::PowerUpMaterial = (UMaterial*)PowerUpMaterialInstance.Object;
}


