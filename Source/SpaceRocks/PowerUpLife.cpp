// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceRocks.h"
#include "PowerUpLife.h"

APowerUpLife::APowerUpLife() {
	// Set PowerUp material:
	static ConstructorHelpers::FObjectFinder<UMaterial>PowerUpMaterialInstance(TEXT("Material'/Game/Art/Materials/PowerUpLifeMaterial.PowerUpLifeMaterial'")); // Get a reference to an asset in the content browser
	if (PowerUpMaterialInstance.Succeeded())
		Super::PowerUpMaterial = (UMaterial*)PowerUpMaterialInstance.Object;
}


