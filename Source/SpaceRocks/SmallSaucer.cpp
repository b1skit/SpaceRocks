// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceRocks.h"
#include "SRSaucerProjectile.h"
#include "SpaceRocksGameMode.h"	// For retrieving the game mode
#include "SmallSaucer.h"

// Constructor
ASmallSaucer::ASmallSaucer() {
	FVector Scale = FVector(0.75f);
	this->GetSaucerMesh()->SetWorldScale3D(Scale);

	this->GetSaucerCollision()->InitSphereRadius(52.0f);

	// Set the point value
	this->SetPointValue(1000);
}

void ASmallSaucer::FireShot() {
	if (this->GetCanFire()) {
		FRotator FireRotation(0, 0, 0);
		// Get the angle to the player ship:
		ASpaceRocksGameMode *TheGameMode = Cast<ASpaceRocksGameMode>(UGameplayStatics::GetGameMode(this));
		if (TheGameMode != NULL) {
			APlayerShip* ThePlayerShip = TheGameMode->GetPlayerShip(); // Get the player ship
			if (ThePlayerShip != NULL && ThePlayerShip->IsValidLowLevel() && !ThePlayerShip->IsPendingKill()) {

				FireRotation = (ThePlayerShip->GetActorLocation() - this->GetActorLocation()).Rotation();
				// Add some random jitter, based on the current game level
				float FiringJitter = MAX_FIRING_JITTER - TheGameMode->GetCurrentLevel();
				if (FiringJitter < 0) // 100% accuracy
					FiringJitter = 0.0f;
				FireRotation.Yaw += FMath::RandRange(-FiringJitter, FiringJitter);
			}
		}
		else { // Pick a random rotation if the player is dead
			FireRotation = FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f);
		}

		// Spawn projectile at an offset from this pawn
		const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(WeaponOffset);

		UWorld* const World = GetWorld();
		if (World != NULL) {
			// spawn the projectile Actor
			World->SpawnActor<ASRSaucerProjectile>(SpawnLocation, FireRotation);

			// Set a timer to toggle to "can fire" flag
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &ASaucer::ShotTimerExpired, FIRE_RATE);
		}



		// Disable firing (for now...)
		this->SetCanFire(false);

		

		// Try and play the sound (if specified)
		if (this->GetSaucerFire() != NULL) {
			UGameplayStatics::PlaySoundAtLocation(this, this->GetSaucerFire(), GetActorLocation());
		}
	}
}

