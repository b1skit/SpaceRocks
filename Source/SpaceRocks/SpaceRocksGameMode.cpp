// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceRocks.h"				// Contains global screen boundary values
#include "PlayerShip.h"				// Require to spawn player ships
#include "Kismet/GameplayStatics.h"
#include "SpaceRocksGameMode.h"

// Constructor
ASpaceRocksGameMode::ASpaceRocksGameMode() {
	
	// Set the current state: (This also initializes the game mode variables)
	SetCurrentState(ESpaceRocksPlayState::EStartScreen);

	// Flag the respawn state:
	bAwaitingRespawn = true;

	// Initialize the free life counter
	PointsToFreeLife = 0;
}

// Tick: Called every frame
// This monitors the current game state and coordinates everything that needs to happen
void ASpaceRocksGameMode::Tick(float DeltaSeconds) {
	// An (unallocated) pointer to the player ship
	APlayerShip* ThePlayer;

	// Check the current state:
	switch(CurrentState) {
	// START SCREEN:
	case ESpaceRocksPlayState::EStartScreen:
		// Display front end menu
		break;

	// NEW GAME:
	case ESpaceRocksPlayState::ENewGame:
					
		break;

	// PLAYING:
	case ESpaceRocksPlayState::EPlaying:
		// Get the player ship:
		ThePlayer = Cast<APlayerShip>(UGameplayStatics::GetPlayerPawn(this, 0));

		// Player is dead and has lives left (needs to be respawned):
		if (ThePlayer == NULL && PlayerLives > 0) {

			UE_LOG(LogTemp, Warning, TEXT("Player dead! %d lives left & game still playing: Respawning!"), PlayerLives);

			// Set a timer to respawn the player after a delay:
			UWorld* const World = GetWorld();
			if (World != NULL) {
				World->GetTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASpaceRocksGameMode::SpawnPlayer, RESPAWN_DELAY);	
			}
			// Adjust the game state:
			SetCurrentState(ESpaceRocksPlayState::ERespawning);
		}

		// Player is dead and has no lives left (Game over):
		else if (ThePlayer == NULL && PlayerLives <= 0) {
			SetCurrentState(ESpaceRocksPlayState::EGameOver); // Set/handle the game over state
		}

		// Level over:
		else if (TheAsteroidSpawner != NULL && TheAsteroidSpawner->GetAsteroidCount() <= 0 && PlayerLives > 0) {
			SetCurrentState(ESpaceRocksPlayState::EEndOfLevel); // Set/handle the end of level state
		}
		
		break;

	// RESPAWNING:
	case ESpaceRocksPlayState::ERespawning:
		// Do nothing, for now.......
		break;

	// END OF LEVEL:
	case ESpaceRocksPlayState::EEndOfLevel:
		

		// TEMP WORKAROUND:
		// Set the state to "Playing" to start the next level
		SetCurrentState(ESpaceRocksPlayState::EPlaying);
		break;


	// GAME OVER:
	case ESpaceRocksPlayState::EGameOver:
		
		//// TEMP WORKAROUND:
		//// Set the state to "Start Screen"
		//SetCurrentState(ESpaceRocksPlayState::EStartScreen);
		
		break;

	
	// UNKNOWN/DEFAULT:
	case ESpaceRocksPlayState::EUnknown:
	default:

		break;
	}

}

// Begin play:
void ASpaceRocksGameMode::BeginPlay() {


}

// Set the current game state
void  ASpaceRocksGameMode::SetCurrentState(ESpaceRocksPlayState NewState) {
	CurrentState = NewState;

	HandleNewState(NewState);
}

// Handle a new game state:
void ASpaceRocksGameMode::HandleNewState(ESpaceRocksPlayState NewState) {
	switch (NewState) {
		// Handle the start screen and prepare for a new game. Should be called at the beginning of all new games!
		case ESpaceRocksPlayState::EStartScreen:
			UE_LOG(LogTemp, Warning, TEXT("Handling StartScreen!"));

			// Display start menu etc
			// ..... (TBC)


			// (Re)Initialize player lives, score, level etc (FOR NEW GAME)
			PlayerLives = STARTING_LIVES;
			PlayerPoints = 0;
			CurrentLevel = 0;
			break;

		// Handle the new game state
		case ESpaceRocksPlayState::ENewGame:
			UE_LOG(LogTemp, Warning, TEXT("Handling New Game!!"));
			// Initialize the asteroid spawner:
			if (TheAsteroidSpawner != NULL) {
				
				UE_LOG(LogTemp, Warning, TEXT("DEBUG: SpaceRocksGameMode::HandleNewState Case newgame fired!!"));

				TheAsteroidSpawner->StartSpawning();
			}
			else
				UE_LOG(LogTemp, Warning, TEXT("DEBUG: SpaceRocksGameMode::HandleNewState Case newgame TheAsteroidSpawner is NULL!!"));

			// (Re)Initialize the free life counter
			PointsToFreeLife = 0;

			// Move the game to the playing state
			SetCurrentState(ESpaceRocksPlayState::EPlaying);
			break;

		// Handle the gameplay state. Set at the begging of each level, until the player dies or completes the level
		case ESpaceRocksPlayState::EPlaying:
			UE_LOG(LogTemp, Warning, TEXT("Handling Playing!"));

			// Trigger new wave of asteroids
			if (TheAsteroidSpawner != NULL && !bAwaitingRespawn) {
				TheAsteroidSpawner->SpawnNextWave();
				
				// Increment the level number:
				CurrentLevel++;
			}

			// Flag the respawn state
			bAwaitingRespawn = false;

			break;
		
		case ESpaceRocksPlayState::ERespawning:
			bAwaitingRespawn = true;
			break;

		// Handle the end-of-level state. This is the state between finishing the current level, and beginning the next leve.
		case ESpaceRocksPlayState::EEndOfLevel:
			UE_LOG(LogTemp, Warning, TEXT("Handling End of Level!!"));

			break;

		// Handle the game over state. The player has died. Handle high scores etc before transitioning to the main menu
		case ESpaceRocksPlayState::EGameOver:
			UE_LOG(LogTemp, Warning, TEXT("Handling Game Over!!"));
			// Display the end of game UI ?
			// .....(TBC)

			
			// Reset the Asteroid Spawner:
			if(TheAsteroidSpawner != NULL)
				TheAsteroidSpawner->ResetSpawner();

			// Display end game UI ????

			// Load the menu level:
			
			//UGameplayStatics::OpenLevel(FString(""));
			GetWorld()->ServerTravel(FString("World'/Game/UI/StartScreen/MainMenu.MainMenu'"));
			
			break;
	
		// Handle the unknown/default state: This should never be called!
		case ESpaceRocksPlayState::EUnknown:
		default:
			// ???? Maybe display a warning message?
			break;

	}
}

// Handle a player death
void ASpaceRocksGameMode::PlayerDied() {
	PlayerLives--;
}

// Add points:
void ASpaceRocksGameMode::AddPoints(uint16 NewPoints) {
	PlayerPoints += NewPoints;

	PointsToFreeLife += NewPoints;
	if (PointsToFreeLife > FREE_LIFE_POINTS) {
		this->AddLife(); // Add an extra life
		PointsToFreeLife = 0;

		if (ThePlayerShip != NULL)
			ThePlayerShip->PlayOneUpSound();
	}

}

// Register the Asteroid spawner with the game mode
void ASpaceRocksGameMode::RegisterAsteroidSpawner(AAsteroidSpawner* NewAsteroidSpawner) {
	TheAsteroidSpawner = NewAsteroidSpawner;
}

// (re)Spawn a player
void ASpaceRocksGameMode::SpawnPlayer() {

	UWorld* const World = GetWorld();
	if (World != NULL) {
		// (re)Spawn the player:
		World->SpawnActor<APlayerShip>(*(new FVector(1, 1, 0)), *(new FRotator(0, -90.0f, 0.0f)));
		
		// Adjust the game state:
		SetCurrentState(ESpaceRocksPlayState::EPlaying);
	}
}

// Register the player ship with the game mode
void ASpaceRocksGameMode::RegisterPlayerShip(APlayerShip* NewPlayerShip) {
	ThePlayerShip = NewPlayerShip;
}

// A blueprint callable function to trigger game start from the menu
//UFUNCTION(BlueprintCallable, Category = "SpaceRocksGame")
void ASpaceRocksGameMode::BPStartSRGame() {
	SetCurrentState(ESpaceRocksPlayState::ENewGame); // New game state triggers playing state
}