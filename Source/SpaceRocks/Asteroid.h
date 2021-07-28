// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AsteroidSpawner.h"
#include "GameFramework/Pawn.h"
#include "Asteroid.generated.h"

// Enumerator to name each Asteroid's randomly generated rotation axis
enum class EAsteroidRotationDirection : short {
	EPitch,
	EYaw,
	ERoll
};

UCLASS()
class SPACEROCKS_API AAsteroid : public APawn
{
	GENERATED_BODY()

public:
	// Constructor
	AAsteroid();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Tick: Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Handle the Asteroid hitting something
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	//UFUNCTION()
	void AddPoints();

	// Record the Asteroid's spawner object
	UFUNCTION()
	void RegisterParentSpawner(AAsteroidSpawner* ParentSpawner);


	// Asteroid appearance variables:
	UStaticMeshComponent *AsteroidMesh;				// Asteroid visual static mesh component. Must be public for child access.
	
	// Asteroid collision sphere:
	USphereComponent *AsteroidCollision;			// Asteroid simple sphere collision component. Must be public for child access.

	// Set the Asteroid's color, and update's the dynamic material
	void SetColorModifier(float NewColorModifier);	

	// Get the Asteroid's randomly generated color modifier (ie. to give it to a shatter)
	float GetColorModifier();

	UProjectileMovementComponent* AsteroidMovement;	// Asteroid movement component

	// Returns a float between min and max Asteroid scale
	float GetRandomScaleValue();

	// Asteroid material
	UMaterial *AsteroidMaterial;						// Base material

private:
	// Asteroid speed variables:
	float MinAsteroidSpeed;		// The minimum speed a LARGE Asteroid could possibly go
	float MaxAsteroidSpeed;		// The maximum speed a LARGE Asteroid could possibly go
	
	// Asteroid size variables:
	const float MIN_ASTEROID_SCALE = 0.8f;
	const float MAX_ASTEROID_SCALE = 1.0f;
	
	// Asteroid rotation variables
	const float MIN_ASTEROID_ROTATION = -10.0f;
	const float MAX_ASTEROID_ROTATION = 10.0f;

	float AsteroidSpeed;

	float AsteroidShieldBounceMultiplier = 2.0f;

	const float ASTEROID_WONKINESS = 10.0f;
	float AsteroidRotationRate;
	EAsteroidRotationDirection AsteroidRotationDirection;

	// Asteroid material variables:
	UMaterialInstanceDynamic *DynamicAsteroidMaterial;	// Dynamic instance material
	float AsteroidColorModifier;						// Color modifier value: Used to dynamically modify the material
	const float MIN_ASTEROID_COLOR_MODIFIER = 1.0f;		// Color modifier min
	const float MAX_ASTEROID_COLOR_MODIFIER = 100.0f;	// Color modifier max

	// Asteroid physics and movement variables:
	FVector CurrentVelocity;						// The Asteroid's current velocity	
	
	// Particles
	UParticleSystem *AsteroidParticles;

	// The Spawner who spawned this Asteroid (Should be initialized by spawner calling this function immediately after construction)
	AAsteroidSpawner* TheAsteroidSpawner;			

	// Asteroid point values
	const int LARGE_ASTEROID_POINTS = 20;
	const int MEDIUM_ASTEROID_POINTS = 50;
	const int SMALL_ASTEROID_POINTS = 100;

	// Generates a random starting speed for an Asteroid
	// Returns a float between min and max Asteroid speed
	float GetStartingSpeed();

	// Returns a float between min and max Asteroid rotation
	float GetRandomRotationValue();

	// Returns a float between min and max Asteroid saturation
	float GetRandomColorModifierValue();

	// Get a random EAsteroidRotationDirection
	EAsteroidRotationDirection GetRandomRotationDirection();
};
