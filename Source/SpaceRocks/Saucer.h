// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Saucer.generated.h"

UCLASS()
class SPACEROCKS_API ASaucer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASaucer();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Handle the Saucer hitting something
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Handler for the fire timer expiry
	void ShotTimerExpired();

	// Get the fire state
	uint32 GetCanFire();

	// Set the fire state
	void SetCanFire(uint32 NewCanFire);

	// Get the static mesh
	UStaticMeshComponent* GetSaucerMesh();

	// Get the collision
	USphereComponent* GetSaucerCollision();

	// Get the firing sound effect
	USoundBase* GetSaucerFire();

	// Set the point value
	void SetPointValue(uint16 NewPointValue);

private:
	uint16 PointValue;

	USphereComponent *SaucerCollision;	// Physical collision mesh
	UStaticMeshComponent *SaucerMesh;	// Visible Saucer mesh
	
	// Saucer movement variables
	FVector CurrentVelocity;
	const float MAX_SAUCER_SPEED = 500;
	const float SAUCER_ACCELERATION = 50.0f; // Rate to change Saucer velocity at
	// Updates CurrentVelocity to follow the player ship, if neccessary. Called during tick
	void FollowPlayerShip();
	// Dodge nearby obstacles (Asteroids, player ship)
	void DodgeNearby();

	// Saucer AI variables
	const float MIN_IDEAL_DISTANCE_TO_PLAYER = 700; // How close the saucer should be to the player before it stops accelerating towards them
	const float MAX_DISTANCE_TO_ASTEROID = 450; // How close saucer needs to be to an obstacle before trying to avoid it

	// Generate a random starting velocity
	FVector GetStartingVelocity();

	// Saucer weapons
	FVector WeaponOffset = FVector(120.0f, 0.0f, 0.0f);
	FTimerHandle TimerHandle_ShotTimerExpired;	// Handle for efficient management of ShotTimerExpired timer
	virtual void FireShot();
	const float FIRE_RATE = 1.0f;				// How fast the weapon will fire
	uint32 bCanFire;
	
	FRotator GetRandomShotRotation();

	// Sound effects
	USoundBase *SaucerHit;
	USoundBase *SaucerFire;	
	USoundBase *HitSound;						// Sound to play when the saucer is destroyed by something

	// Particles
	UParticleSystem *SaucerDestructionParticles;
};

// Get the fire state
FORCEINLINE uint32 ASaucer::GetCanFire() {
	return bCanFire;
}

// Set the fire state
FORCEINLINE void ASaucer::SetCanFire(uint32 NewCanFire) {
	bCanFire = NewCanFire;
}

// Get the static mesh
FORCEINLINE UStaticMeshComponent* ASaucer::GetSaucerMesh() {
	return SaucerMesh;
}

// Get the collision
FORCEINLINE USphereComponent* ASaucer::GetSaucerCollision() {
	return SaucerCollision;
}

FORCEINLINE USoundBase* ASaucer::GetSaucerFire() {
	return SaucerFire;
}

// Set the point value
FORCEINLINE void ASaucer::SetPointValue(uint16 NewPointValue) {
	PointValue = NewPointValue;
}