// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceRocks.h"
#include "AsteroidMedium.h"


// Constructor
AAsteroidMedium::AAsteroidMedium() {

	// Load the Asteroid static mesh:
	static ConstructorHelpers::FObjectFinder<UStaticMesh> AsteroidMeshAsset(TEXT("StaticMesh'/Game/Art/Models/AsteroidShatter.AsteroidShatter'")); // Create a "FObjectFinder" object named "AsteroidMeshAsset"...
	if (AsteroidMeshAsset.Succeeded()) { // and if successful,
		AsteroidMesh->SetStaticMesh(AsteroidMeshAsset.Object);	// Set the mesh as the object
	}

	// Scale the Asteroid mesh randomly, for variation:
	AsteroidMesh->SetWorldScale3D(FVector(this->GetRandomScaleValue(), this->GetRandomScaleValue(), this->GetRandomScaleValue()) * MediumMeshScaleFactor );

	// Set the parent Asteroid material to be used with a dynamic material instance, for color variation:
	static ConstructorHelpers::FObjectFinder<UMaterial>AsteroidMaterialInstance(TEXT("Material'/Game/Art/Materials/AsteroidShatterMaterial.AsteroidShatterMaterial'")); // Get a reference to an asset in the content browser
	if (AsteroidMaterialInstance.Succeeded())
		AsteroidMaterial = (UMaterial*)AsteroidMaterialInstance.Object;		// Set the material

	if (AsteroidCollision != NULL)
		AsteroidCollision->InitSphereRadius(75.0f);			// Set the size of the collision mesh sphere

}