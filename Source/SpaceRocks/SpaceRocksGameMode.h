// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AsteroidSpawner.h"				// Required to return Asteroid Spawner references
#include "PlayerShip.h"						// Required to return Player Ship references
#include "GameFramework/GameMode.h"
#include "SpaceRocksGameMode.generated.h"

enum class ESpaceRocksPlayState : short {
	EStartScreen,
	ENewGame, // Initializes the new game
	EPlaying,
	ERespawning,
	EEndOfLevel,
	EGameOver,
	EUnknown
};

/**
 * 
 */
UCLASS(Blueprintable)
class SPACEROCKS_API ASpaceRocksGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	// Constructor
	ASpaceRocksGameMode();

	// Game tick:
	virtual void Tick(float DeltaSeconds) override;
	
	// Called at the start of the game
	void BeginPlay() override;

	// Game variables:
	// Number of starting lives:
	const int STARTING_LIVES = 3;

	// The current number of lives
	UPROPERTY(BlueprintReadWrite, Category="HUD")
	int PlayerLives;

	// The current number of points
	//UPROPERTY(BlueprintReadWrite, Category="HUD")
	uint32 PlayerPoints;


	/********** GAME STATE HANDLING: ************************/
	// Get the current game state
	ESpaceRocksPlayState GetCurrentState() const;

	// Set the current game state
	void  SetCurrentState(ESpaceRocksPlayState NewState);

	// A blueprint callable function to trigger game start from the menu
	UFUNCTION(BlueprintCallable, Category = "SpaceRocksGame")
	void BPStartSRGame();

	// Handle a new game state
	void HandleNewState(ESpaceRocksPlayState NewState);

	// Handle a player death
	void PlayerDied();

	// (re)Spawn a player
	void SpawnPlayer();


	/********** GAME INFO: *******************************/
	// Get the player score
	UFUNCTION(BlueprintCallable, Category="HUD")
	int	GetPlayerPoints();

	// Get the number of player lives
	//UFUNCTION(BlueprintPure, BlueprintCallable, Category = "HUD")
	UFUNCTION(BlueprintCallable, Category = "HUD")
	int	GetPlayerLives();

	// Add points
	void AddPoints(uint16 NewPoints);

	// Get the current gameplay level (Blueprint callable)
	UFUNCTION(BlueprintCallable, Category = "HUD")
	int GetCurrentLevel();

	/********** SPAWNER INTERACTION: *********************/
	// Register the Asteroid spawner with the game mode
	void RegisterAsteroidSpawner(AAsteroidSpawner* NewAsteroidSpawner);

	// Get the Asteroid spawner
	AAsteroidSpawner* GetAsteroidSpawner();

	// Register the player ship with the game mode
	void RegisterPlayerShip(APlayerShip* ThePlayerShip);

	// Get the Player ship
	APlayerShip* GetPlayerShip();

	// Get the Player ship shield value
	UFUNCTION(BlueprintCallable, Category = "HUD")
	float GetPlayerShipShieldValue();

	// Add an extra life
	void AddLife();

private:
	// The current game state
	ESpaceRocksPlayState CurrentState;

	// The active asteroid spawner
	AAsteroidSpawner* TheAsteroidSpawner; 

	// The active player ship
	APlayerShip* ThePlayerShip;

	// The current player level
	uint32 CurrentLevel;

	uint32 bAwaitingRespawn;

	const uint32 FREE_LIFE_POINTS = 10000;	// At what level to award a free life
	uint32 PointsToFreeLife;

	// Respawn timer: Handles player respawn delay
	FTimerHandle TimerHandle_RespawnTimer;
	const float RESPAWN_DELAY = 1.0f;

};

FORCEINLINE ESpaceRocksPlayState ASpaceRocksGameMode::GetCurrentState() const {
	return CurrentState;
}

// Get the current number of player points (as an int)
FORCEINLINE int ASpaceRocksGameMode::GetPlayerPoints(){
	return PlayerPoints;
}

// Get the current number of player lives remaining (as an int)
FORCEINLINE int ASpaceRocksGameMode::GetPlayerLives(){
	return PlayerLives;
}

// Get the Player ship
FORCEINLINE APlayerShip* ASpaceRocksGameMode::GetPlayerShip(){
	return ThePlayerShip;
}

// Get the Asteroid spawner
FORCEINLINE AAsteroidSpawner* ASpaceRocksGameMode::GetAsteroidSpawner() {
	return TheAsteroidSpawner;
}

// Get the current gameplay level
FORCEINLINE int ASpaceRocksGameMode::GetCurrentLevel() {
	return CurrentLevel;
}

// Get the Player ship shield value
FORCEINLINE float ASpaceRocksGameMode::GetPlayerShipShieldValue() {
	if (ThePlayerShip != NULL) {
		int CurrentShieldValue = ThePlayerShip->GetShieldValue();

		if (CurrentShieldValue == 0)
			return 0;
		else
			return (float)CurrentShieldValue / 100.0;
	}
	else
		return 0;
}

// Add an extra life
FORCEINLINE void ASpaceRocksGameMode::AddLife() {
	PlayerLives++;
}