// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceRocks.h"				// Contains global screen boundary values
#include "Asteroid.h"
#include "AsteroidLarge.h"			// Required to spawn large asteroids
#include "AsteroidMedium.h"
#include "AsteroidSmall.h"
#include "SmallSaucer.h"
#include "PowerUp.h"
#include "PowerUpShield.h"
#include "PowerUpLife.h"
#include "PowerUpPoints.h"
#include "PowerUpWeapon.h"
#include "SpaceRocksGameMode.h"		// Required to check game mode status
#include "Kismet/GameplayStatics.h"	// Required to retrieve game mode
#include "AsteroidSpawner.h"


// Constructor
AAsteroidSpawner::AAsteroidSpawner()
{
	// Create a dummy root component we can attach things to.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Create a visible object (To use as an editor mesh ONLY - Spawner should be set to "Do not render in game" within editor)
	SpawnerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnerMesh")); // A USceneComponent* pointing to a UStaticMeshComponent
	
	// Attach the mesh
	SpawnerMesh->AttachTo(RootComponent);

	// Initialize the current number of Asteroids in play
	AsteroidCount = 0;
	
	// Configure the Asteroid spawn parameters (Don't fail spawning because of collision)
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
}

// Called when the game starts or when spawned
void AAsteroidSpawner::BeginPlay()
{
	Super::BeginPlay();

	// Get the current Game Mode
	ASpaceRocksGameMode *TheGameMode = Cast<ASpaceRocksGameMode>(UGameplayStatics::GetGameMode(this));

	// Register the Asteroid Spawner with the game mode:
	if (TheGameMode != NULL)
		TheGameMode->RegisterAsteroidSpawner(this);
	else
		UE_LOG(LogTemp, Warning, TEXT("AAsteroidSpawner::BeginPlay Failed to register spawner!!!!!"));
}

// Spawn a Saucer
void AAsteroidSpawner::SpawnSaucer() {

	UE_LOG(LogTemp, Warning, TEXT("AAsteroidSpawner::SpawnSaucer Called!!!!"));

	ASpaceRocksGameMode *TheGameMode = Cast<ASpaceRocksGameMode>(UGameplayStatics::GetGameMode(this)); // Get the game mode
	UWorld* World = GetWorld();
	if (World != NULL && TheGameMode != NULL) {

		// Spawn Saucer:
			
		// Set the spawn parameters: Don't fail spawning because of collision:
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// Attempt to spawn a Saucer, as long as no saucer currently exists
		if (CurrentSaucer == NULL) {
			FVector *SaucerSpawnLocation = GetSpawnLocation();
			if (TheGameMode->GetPlayerPoints() < SmallSaucerPointLevel) {
				CurrentSaucer = World->SpawnActor<ASaucer>(*SaucerSpawnLocation, FRotator(0, 0, 0), SpawnInfo);
			}
			else
				CurrentSaucer = World->SpawnActor<ASmallSaucer>(*SaucerSpawnLocation, FRotator(0, 0, 0), SpawnInfo);
		}		

		// Set a timer to start next saucer spawn. 
		float NewTimer = (StartingSaucerFrequency - TheGameMode->GetCurrentLevel() ) > MaxSaucerFrequency? StartingSaucerFrequency - TheGameMode->GetCurrentLevel() : MaxSaucerFrequency;

		World->GetTimerManager().SetTimer(TimerHandle_SpawnSaucerTimer, this, &AAsteroidSpawner::SpawnSaucer, StartingSaucerFrequency - TheGameMode->GetCurrentLevel() );
	}
}

// Get a random spawn location
FVector* AAsteroidSpawner::GetSpawnLocation(){

	// Randomly generate a spawn location within play bounds:
	FVector *NewSpawnLocation = new FVector(FMath::FRandRange(ScreenXMin, ScreenXMax), FMath::FRandRange(ScreenYMin, ScreenYMax), 0.0f);

	// Check the spawn location, and adjust as neccessary:
	ASpaceRocksGameMode *TheGameMode = Cast<ASpaceRocksGameMode>(UGameplayStatics::GetGameMode(this)); // Get the Game Mode
	if(TheGameMode != NULL){
		APlayerShip *ThePlayerShip = TheGameMode->GetPlayerShip(); // Get the player ship
		if (ThePlayerShip != NULL) { 
			FVector PlayerShipLocation = ThePlayerShip->GetActorLocation(); // Get the player ship location
						
			if (FMath::Abs(NewSpawnLocation->X - PlayerShipLocation.X) < PlayerXBuffer) { // Check we're not spawning too close
				if (NewSpawnLocation->X < 0) {  // Negative X value
					NewSpawnLocation->X += ScreenXMax;
				}
				else // Positive X value
					NewSpawnLocation->X += ScreenXMin;
			}
			if (FMath::Abs(NewSpawnLocation->Y - PlayerShipLocation.Y) < PlayerYBuffer) { // Check we're not spawning too close
				if (NewSpawnLocation->Y < 0) {  // Negative Y value
					NewSpawnLocation->Y += ScreenYMax;
				}
				else // Positive Y value
					NewSpawnLocation->Y += ScreenYMin;
			}
		}
		else {	// No valid player ship: Spawn away from screen center based on a generic buffer zone
			if (NewSpawnLocation->X > -PlayerXBuffer && NewSpawnLocation->X < PlayerXBuffer) { // Check we're not spawning too close
				if (NewSpawnLocation->X < 0) {  // Negative X value
					NewSpawnLocation->X += ScreenXMax;
				}
				else // Positive X value
					NewSpawnLocation->X += ScreenXMin;
			}
			if (NewSpawnLocation->Y > -PlayerYBuffer && NewSpawnLocation->Y < PlayerYBuffer) { // Check we're not spawning too close
				if (NewSpawnLocation->Y < 0) {  // Negative Y value
					NewSpawnLocation->Y += ScreenYMax;
				}
				else // Positive Y value
					NewSpawnLocation->Y += ScreenYMin;
			}
		}
	}
	
	// Return the spawn vector:
	return NewSpawnLocation;
}

// Get a random spawn rotation
FRotator* AAsteroidSpawner::GetSpawnRotation() {
	FRotator *NewSpawnRotation = new FRotator;
	NewSpawnRotation->Pitch = 0.0f;
	NewSpawnRotation->Yaw = FMath::FRandRange(0, 360);
	NewSpawnRotation->Roll = 0.0f;

	return NewSpawnRotation;
}

// Initialize the spawner for a new game
void AAsteroidSpawner::StartSpawning() {

	// Set a timer to start a saucer spawing cycle
	UWorld* World = GetWorld();
	if (World != NULL) {
		if (TimerHandle_SpawnSaucerTimer.IsValid()) // Clear the current timer, if it's active
			World->GetTimerManager().ClearTimer(TimerHandle_SpawnSaucerTimer);

		UE_LOG(LogTemp, Warning, TEXT("DEBUG: AAsteroidSpawner::StartSpawning() Called!!!!"));

		// Set a new timer
		World->GetTimerManager().SetTimer(TimerHandle_SpawnSaucerTimer, this, &AAsteroidSpawner::SpawnSaucer, StartingSaucerFrequency);
	}
}

// Spawn next wave of Asteroids:
void AAsteroidSpawner::SpawnNextWave() {

	// Get the Game Mode:
	ASpaceRocksGameMode *TheGameMode = Cast<ASpaceRocksGameMode>(UGameplayStatics::GetGameMode(this)); 
	
	// Spawn the required number of Asteroids:
	for (uint8 i = 0; i < (StartingLargeAsteroids + (uint8)(TheGameMode->GetCurrentLevel() * AsteroidGrowthFactor) ) && i < MaxAsteroids; i++) { // Number of asteroids = floor( Growth factor * current level) + starting asteroids 
		// Generate spawn location for Asteroid
		FVector *AsteroidSpawnLocation = GetSpawnLocation();
		FRotator *AsteroidSpawnRotation = GetSpawnRotation();

		// Get the world:
		UWorld* const World = GetWorld();
		if (World != NULL) { // Spawn an Asteroid:
			// Attempt to spawn the new Asteroid:
			AAsteroid *NewAsteroid = World->SpawnActor<AAsteroidLarge>(*AsteroidSpawnLocation, *AsteroidSpawnRotation, SpawnInfo);

			if (NewAsteroid != NULL) {	// Successful Asteroid spawn! 
				AsteroidCount++;		// Increment the current Asteroid count
				NewAsteroid->RegisterParentSpawner(this);	// Register this spawner with the Asteroid

				TheAsteroids.Emplace(NewAsteroid); // Add the Asteroid pointer to the array
			}
			else { // Asteroid spawn failed
				UE_LOG(LogTemp, Warning, TEXT("AAsteroidSpawner::SpawnNextWave() - Spawn Error!"));
			}
		}
	}
}

// Spawn smaller Asteroid "shatters"
void AAsteroidSpawner::SpawnShatter(FVector ParentLocation, FRotator ParentRotation, AAsteroid* AsteroidParent){
	// Get the World:
	UWorld* World = AActor::GetWorld();

	// Randomly spawn a powerup
	if (World != NULL && FMath::RandRange(0.0f, 1.0f) < PowerUpChance && ActivePowerUp == NULL) {
		uint8 PowerUpType = FMath::RandRange(0, 3); // Choose which powerup to spawn
		switch (PowerUpType) {
		case 0: // Shield power up
			ActivePowerUp = World->SpawnActor<APowerUpShield>(ParentLocation, FRotator(0, 0, 0));
			break;

		case 1: // Extra life power up
			ActivePowerUp = World->SpawnActor<APowerUpLife>(ParentLocation, FRotator(0, 0, 0));
			break;

		case 2: // Extra points power up
			ActivePowerUp = World->SpawnActor<APowerUpPoints>(ParentLocation, FRotator(0, 0, 0));
			break;

		case 3: // Weapon upgrade power up
			ActivePowerUp = World->SpawnActor<APowerUpWeapon>(ParentLocation, FRotator(0, 0, 0));
			break;

		default: break;
		}
		

	}

	if (AsteroidParent != NULL && !AsteroidParent->IsA(AAsteroidSmall::StaticClass())) { // Don't attempt spawn if this Asteroid is small or the parent is NULL
		// Loop for every shatter to be spawned:
		for (uint8 i = 0; i < NumShatters; i++) { 
			
			if (World != NULL && AsteroidParent != NULL) {			
				// Spawn relevant new Asteroid:
				AAsteroid* NewAsteroid = NULL;
				if (AsteroidParent->IsA(AAsteroidLarge::StaticClass())) {			// Large Asteroid shattering
					NewAsteroid = World->SpawnActor<AAsteroidMedium>(ParentLocation, ParentRotation, SpawnInfo);
				}
				else if (AsteroidParent->IsA(AAsteroidMedium::StaticClass())) {		// Medium Asteroid shattering
					NewAsteroid = World->SpawnActor<AAsteroidSmall>(ParentLocation, ParentRotation, SpawnInfo);
				}

				// If spawn successful, handle the new Asteroid:
				if (NewAsteroid != NULL) {
					AsteroidCount++;													// Update the count
					NewAsteroid->RegisterParentSpawner(this);							// Register the spawner with the new Asteroid
					NewAsteroid->SetColorModifier(AsteroidParent->GetColorModifier());	// Set the shatter to be the same color as the parent
					TheAsteroids.Emplace(NewAsteroid);									// Add the Asteroid to the Asteroid array
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("WARNING! AAsteroidSpawner::SpawnShatter NewAsteroid is NULL!"));
				}
			} // End World, AsteroidParent NULL check
		} // End for loop
	} // End of NULL check
}

// De-increment the AsteroidCount (ie. An asteroid has been destroyed)
void AAsteroidSpawner::AsteroidDestroyed(AAsteroid *DestroyedAsteroid) {
	
	int32 Index;
	if (TheAsteroids.Find(DestroyedAsteroid, Index)) {
		TheAsteroids.Remove(DestroyedAsteroid);

		DestroyedAsteroid->Destroy();
	}

	// De-increment the Asteroid count:
	AsteroidCount--; 
}

// Deregister the saucer
void AAsteroidSpawner::SaucerDestroyed() {
	CurrentSaucer = NULL;
}

// Delete all Asteroids and saucers: Reset the spawner to start screen condition
void AAsteroidSpawner::ResetSpawner() {
	// Destroy the Asteroids we're tracking:
	if (TheAsteroids.Num() > 0) {
		for (AAsteroid *Current : TheAsteroids) {
			if (Current->IsValidLowLevel() && !Current->IsPendingKill())
				Current->Destroy();	// Destroy the Asteroid (This will update the containing TArray automatically)
		}
	}
	TheAsteroids.Empty(); // Empty the array of all references

	// Reset the count
	AsteroidCount = 0;

	// Destroy the Saucer, if it exists:
	if (CurrentSaucer != NULL) {
		CurrentSaucer->Destroy();
		CurrentSaucer = NULL; // Clear the saucer
	}
			
	//Reset the Saucer timer:
	if (TimerHandle_SpawnSaucerTimer.IsValid())
		GetWorldTimerManager().ClearTimer(TimerHandle_SpawnSaucerTimer);

	// Destroy any powerups:
	if (ActivePowerUp != NULL) {
		ActivePowerUp->Destroy();
		ActivePowerUp = NULL;	// Clear the power up
	}
		

}

// Add points (passes points to the game mode)
void AAsteroidSpawner::AddPoints(int NewPoints) {
	// Get the game mode:
	ASpaceRocksGameMode *TheGameMode = Cast<ASpaceRocksGameMode>(UGameplayStatics::GetGameMode(this));

	// Add the points
	if (TheGameMode != NULL)
		TheGameMode->AddPoints(NewPoints);
}

// Get minimum Asteroid speed value based on its size
int AAsteroidSpawner::GetMinAsteroidSpeed(AAsteroid* NewAsteroid) {
	if (NewAsteroid->IsA(AAsteroidSmall::StaticClass()))
		return MinSmallAsteroidSpeed;
	else if (NewAsteroid->IsA(AAsteroidMedium::StaticClass()))
		return MinMediumAsteroidSpeed;
	else
		return MinLargeAsteroidSpeed;
}

// Get maximum Asteroid speed value based on its size
int AAsteroidSpawner::GetMaxAsteroidSpeed(AAsteroid* NewAsteroid) {
	if (NewAsteroid->IsA(AAsteroidSmall::StaticClass()))
		return MaxSmallAsteroidSpeed;
	else if (NewAsteroid->IsA(AAsteroidMedium::StaticClass()))
		return MaxMediumAsteroidSpeed;
	else
		return MaxLargeAsteroidSpeed;

}

// PowerUp collected
void AAsteroidSpawner::PowerUpDestroyed() {
	ActivePowerUp = NULL;
}