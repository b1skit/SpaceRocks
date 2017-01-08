// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlayerShip.generated.h"	// Must be last in this list!

/**
 * 
 */
UCLASS()
class SPACEROCKS_API APlayerShip : public APawn
{
	GENERATED_BODY()

public:
	// Input functions
	// Thrust:
	void Move_XAxis(float AxisValue);
	// Rotate the ship:
	void Move_YAxis(float AxisValue);

	// Constructor
	APlayerShip();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// SHIP WEAPONS:
	// Fire a shot
	void FireShot();

	// Handler for the fire timer expiry
	void ShotTimerExpired();

	UFUNCTION() // MUST be a UFUNCTION to work!
	void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/*UPROPERTY(EditAnywhere)*/
	// A visible component for the object
	UStaticMeshComponent *ShipMesh;
	UMaterial *PlayerShipMaterial;						// Base material
	UStaticMeshComponent *MidThrusterMesh;			// Mid thruster
	UStaticMeshComponent *LeftThrusterMesh;				// Mid thruster
	UStaticMeshComponent *RightThrusterMesh;				// Mid thruster
	UMaterial *ThrusterMaterial;
	

	//UPROPERTY(EditAnywhere)
	USphereComponent *ShipCollision;		// Asteroid simple sphere collision component

	// Play the 1-Up sound. Triggered by the game mode as appropriate
	void PlayOneUpSound();

	// PlayerShipThruster sound name references
	static const FName PlayerShipThrusterPitch;
	static const FName PlayerShipThrusterVolume;
	
	// Shield
	// Get the current shield value
	int GetShieldValue();

	// Activate the shield
	void ActivateShield();

	// Deactivate the shield
	void DeactivateShield();

	// Increase the shield value
	void IncreaseShieldValue(uint8 AddValue);
	
	// Handle shield hits
	UFUNCTION() // MUST be a UFUNCTION to work!
	void OnShieldHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	// Get the shield mesh component (used for collision checking)
	UStaticMeshComponent* GetShieldMesh();

private:
	UPROPERTY(EditAnywhere)
	// Factor to scale input by for acceleration/forward thrust
	float ShipAcceleration = 13.0f;
	
	UPROPERTY(EditAnywhere)
	// Factor to scale input by when braking/reverse thrusting
	float ShipBraking = 1.5f;

	UPROPERTY(EditAnywhere)
	// Factor to reduce speed by each tick (speed * drag force)	
	float ShipDrag = 0.996;				// Factor to reduce speed by each tick (speed * drag force)	

	UPROPERTY(EditAnywhere)
	// Factor to scale rotation input
	float RotationSpeed = 4.0f;

	UPROPERTY(EditAnywhere)
	// Max degrees to roll the ship during turning
	float MaxRoll = 32.0f;		

	UPROPERTY(EditAnywhere)
	// How fast to roll the ship mesh during turning
	float RollSpeed = 2.8f;		

	UPROPERTY(EditAnywhere)
	// How fast the weapon will fire
	float FireRate = 0.1f;

	UPROPERTY(EditAnywhere)
	// Amount of time to be invincible for
	float InvincibilityTime = 3.0f;		

	UPROPERTY(EditAnywhere)
	// Time, in seconds, between flashes
	float InvincibilityFlashFrequency = 0.15f; 

	float ThrusterBurnOutTime = 1;		// Starting time/mesh scale for thruster burnout
	UPROPERTY(EditAnywhere)
	// Rate to slow/shrink thruster burning
	float ThrusterFadeRate = 0.01;
	float ThrusterGrowthRate = 0.1;
	float MidThrusterBurnOutRemaining;		// Used to time and scale thruster burnout
	float LeftThrusterBurnOutRemaining;
	float RightThrusterBurnOutRemaining;
	
	// Particles
	UParticleSystem *PlayerShipDestructionParticles;
		
	// Scale the size of the player ship
	uint8 SizeScale = 2; // DEPRECATED: I SHOULD PROBABLY JUST REPLACE THIS WITH A CORRECTLY SCALED MESH AND COLLISION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	// Movement variables:
	FVector CurrentVelocity;				// The ship's current speed (the X/Y/Z directions)

	// Weapon variables:
	uint32 bCanFire : 1;						//Flag to control firing
	uint8 NumSpreadShots;						// The current number of spreadshots
	const uint8 MAX_SPREAD_SHOTS = 4;

	float GunForwardOffset = 70.0f;					// Offset from the ship to spawn projectiles: MUST NOT INTERSECT SHIP!
	float GunForwardStagger = 20.0f;				// Stagger the spread shots by this
	FTimerHandle TimerHandle_ShotTimerExpired;	// Handle for efficient management of ShotTimerExpired timer

	// Shield variables
	float ShieldValue;							// The greatest shield value
	uint8 MaxShieldValue = 100;					// Highest value the shield can be
	uint8 bShieldActivated;
	FTimerHandle TimerHandle_ShieldTimer;
	float ShieldFadeTime = 0.1f;
	float ShieldBurnRate = 7.0f;			// How quickly to decrease the shield value while the shield is active
	UStaticMeshComponent *ShieldMesh;
	UMaterial *PlayerShipShieldMaterial;		// Shield material

	// Sound effects
	USoundBase *FireSound;						// Sound to play each time we fire
	USoundBase *HitSound;						// Sound to play when the ship is destroyed by something
	USoundBase *SpawnSound;						// Sound to play when (re)spawning
	USoundBase *OneUpSound;						// 1-Up sound. Played when triggered by game mode
	
	UAudioComponent* PlayerShipThrusterSoundComponent;

	// (Re)spawn settings
	uint32 bDamageable;							// If the ship can take damage
	FTimerHandle TimerHandle_SpawnInvincibility;
	FTimerHandle TimerHandle_InvincibilityFlash;	// Timer for invincibility flashing

	// Checks if the player ship is damageable
	uint32 IsDamageable(); // Returns bDamageable

	// Toggle visibility on/off (For invincibility "flash" effect)
	void ToggleInvincibilityFlash();

	// Handler for when invincibility expires
	void InvincibilityExpired();

};

// Get the current shield value
FORCEINLINE int APlayerShip::GetShieldValue() {
	return ShieldValue;
}

// Get the shield mesh component (used for collision checking)
FORCEINLINE UStaticMeshComponent* APlayerShip::GetShieldMesh() {
	return ShieldMesh;
}