// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceRocks.h"
#include "AsteroidLarge.h"

// Constructor
AAsteroidLarge::AAsteroidLarge() {
	/*No need to set the Collision Sphere Radius or display mesh for a Large Asteroid, as it is the same as the parent class*/


	// Load the Asteroid static mesh:
	static ConstructorHelpers::FObjectFinder<UStaticMesh> AsteroidMeshAsset(TEXT("/Game/Art/Models/AsteroidLarge.AsteroidLarge")); // Create a "FObjectFinder" object named "AsteroidMeshAsset"...
	if (AsteroidMeshAsset.Succeeded()) { // and if successful,
		AsteroidMesh->SetStaticMesh(AsteroidMeshAsset.Object);	// Set the mesh as the object
	}

	// Scale the Asteroid mesh randomly, for variation:
	AsteroidMesh->SetWorldScale3D(*(new FVector(this->GetRandomScaleValue(), this->GetRandomScaleValue(), this->GetRandomScaleValue())));

	// Set the parent Asteroid material to be used with a dynamic material instance, for color variation:
	static ConstructorHelpers::FObjectFinder<UMaterial>AsteroidMaterialInstance(TEXT("Material'/Game/Art/Materials/AsteroidLargeMaterial.AsteroidLargeMaterial'")); // Get a reference to an asset in the content browser
	if (AsteroidMaterialInstance.Succeeded())
		AsteroidMaterial = (UMaterial*)AsteroidMaterialInstance.Object;		// Set the material
}


