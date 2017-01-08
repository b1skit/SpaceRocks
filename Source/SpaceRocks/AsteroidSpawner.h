// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "Asteroid.h"
#include "GameFramework/Actor.h"
#include "Saucer.h"
#include "PowerUp.h"
#include "AsteroidSpawner.generated.h"

// Forward declarations:
class AAsteroid;

// The Asteroid spawner handles ALL spawning and asteroid count tracking
// Individual asteroids are responsible for reporting score points to the game mode upon destruction
// Asteroid spawner should report to game mode when all asteroids have been destroyed.

UCLASS()
class SPACEROCKS_API AAsteroidSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAsteroidSpawner();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Initialize the spawner for a new game
	void StartSpawning();

	// Spawn next wave of Asteroids
	void SpawnNextWave();

	// Spawn smaller Asteroid "shatters"
	void SpawnShatter(FVector ParentLocation, FRotator ParentRotation, AAsteroid* AsteroidParent);

	// De-increment the AsteroidCount (ie. An asteroid has been destroyed)
	void AsteroidDestroyed(AAsteroid *DestroyedAsteroid);

	// Deregister the saucer
	void SaucerDestroyed();

	// Return the current count of Asteroids in play
	int GetAsteroidCount();

	// Delete all Asteroids and saucers: Reset the spawner to start screen condition
	void ResetSpawner();

	// Add points (passes points to the game mode)
	void AddPoints(int NewPoints);

	// Get the Asteroid array
	TArray<AAsteroid*>* GetAsteroids();

	// Spawn a Saucer
	void SpawnSaucer();

	// Get minimum Asteroid speed value based on its size
	int GetMinAsteroidSpeed(AAsteroid* NewAsteroid);

	// Get maximum Asteroid speed value based on its size
	int GetMaxAsteroidSpeed(AAsteroid* NewAsteroid);

	// PowerUp collected
	void PowerUpDestroyed();

private:
	// GAME SETTINGS:
	UPROPERTY(EditAnywhere)			// Tag the variable so it is visible to the engine (prevents resetting @launch or level reload)
	USceneComponent* SpawnerMesh;	// A visible component for the object
		
	// Number of asteroids to start with in level 1
	UPROPERTY(EditAnywhere)
	uint8 StartingLargeAsteroids = 5; 
	
	// The number of smaller asteroid bits to spawn upon destruction
	UPROPERTY(EditAnywhere)
	uint8 NumShatters = 2;		
	
	UPROPERTY(EditAnywhere)
	// Formula: Floor(StartingLargeAsteroids + (CurrentLevel * AsteroidGrowthFactor) )
	float AsteroidGrowthFactor = 0.5f;

	UPROPERTY(EditAnywhere)
	// The chance powerups will spawn (0, 1)
	float PowerUpChance = 0.5f;

	// Initial frequency for saucer attacks. Increases by a product of the current level
	UPROPERTY(EditAnywhere)
	float StartingSaucerFrequency = 60.0f;

	UPROPERTY(EditAnywhere)
	// The max rate Saucers will spawn
	float MaxSaucerFrequency = 15.0f;

	UPROPERTY(EditAnywhere)
	// The number of points after which small Saucers begin spawning
	uint16 SmallSaucerPointLevel = 10000.0f;

	UPROPERTY(EditAnywhere)
	// The max number of Asteroids possible
	uint8 MaxAsteroids = 75;	

	// AsteroidSpeeds
	UPROPERTY(EditAnywhere)
	// The minimum speed a SMALL Asteroid could possibly go
	float MinSmallAsteroidSpeed = 400.0f;
	
	UPROPERTY(EditAnywhere)
	// The maximum speed a SMALL Asteroid could possibly go
	float MaxSmallAsteroidSpeed = 700.0f;		

	UPROPERTY(EditAnywhere)
	// The minimum speed a MEDIUM Asteroid could possibly go
	float MinMediumAsteroidSpeed = 200.0f;

	UPROPERTY(EditAnywhere)
	// The maximum speed a MEDIUM Asteroid could possibly go
	float MaxMediumAsteroidSpeed = 500.0f;

	UPROPERTY(EditAnywhere)
	// The minimum speed a LARGE Asteroid could possibly go
	float MinLargeAsteroidSpeed = 150.0f;

	UPROPERTY(EditAnywhere)
	// The maximum speed a LARGE Asteroid could possibly go
	float MaxLargeAsteroidSpeed = 300.0f;

	UPROPERTY(EditAnywhere)
	// X Distance (+/-) from player/center to NOT spawn asteroids in
	int16 PlayerXBuffer = 300;	
	
	UPROPERTY(EditAnywhere)
	// Y Distance (+/-) from player/center to NOT spawn asteroids in
	int16 PlayerYBuffer = 225;	

	// ASTEROIDS:
	uint16 AsteroidCount;	// The number of asteroids in play
	TArray<AAsteroid*> TheAsteroids; // Array of pointers to Asteroid's -> Used to track Asteroids in play

	//Generate a spawn point at a safe area in the play space
	FVector* GetSpawnLocation();
	
	//Generate a spawn rotation
	FRotator* GetSpawnRotation();
	
	// SAUCER:
	ASaucer *CurrentSaucer;				// The active saucer in the level
	FTimerHandle TimerHandle_SpawnSaucerTimer;			// Timer that spawns a saucer upon its completion

	// Asteroid spawn parameters
	FActorSpawnParameters SpawnInfo;

	// PowerUps
	APowerUp* ActivePowerUp; // The current powerup
};

// Get the Asteroid array
FORCEINLINE TArray<AAsteroid*>* AAsteroidSpawner::GetAsteroids() {
	return &TheAsteroids;
}

// Return the current count of Asteroids in play
FORCEINLINE int AAsteroidSpawner::GetAsteroidCount() {
	return AsteroidCount;
}