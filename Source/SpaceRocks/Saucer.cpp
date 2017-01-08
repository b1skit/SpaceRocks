// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceRocks.h"			// Contains global screen boundary values
#include "SpaceRocksGameMode.h"	// For retrieving the game mode
#include "AsteroidSpawner.h"	// For retrieving the asteroid spawner
#include "Asteroid.h"			// For retrieving asteroids
#include "SRSaucerProjectile.h"		// For firing projectiles
#include "PlayerShip.h"			// For retrieving information regarding the player ship
#include "Saucer.h"


// Sets default values
ASaucer::ASaucer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set up collision:
	SaucerCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));	// Create a sphere component subobject
	RootComponent = SaucerCollision;													// Use the collision component as the root
	SaucerCollision->InitSphereRadius(69.0f);											// Set the collision sphere radius

	// Collision settings:
	// ECollisionChannel:: Collision profile settings:
	// ECollisionChannel::ECC_GameTraceChannel1 = SRProjectile
	// ECollisionChannel::ECC_GameTraceChannel2 = PlayerShip
	// ECollisionChannel::ECC_GameTraceChannel3 = SRSaucerProjectile
	// ECollisionChannel::ECC_GameTraceChannel4 = Saucer
	//**************************************************************
	SaucerCollision->OnComponentHit.AddDynamic(this, &ASaucer::OnHit);								// Set up a notification for when this component hits something
	SaucerCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	SaucerCollision->SetNotifyRigidBodyCollision(true);												// Same as "Simulation Generates Hit Events" checkbox
	SaucerCollision->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics, true);	// Make sure collision is enabled
	SaucerCollision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel4);
	SaucerCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECR_Ignore); // Ignore the SRSaucerProjectile channel
	
	// Set the Saucer mesh component to hold the static mesh:
	SaucerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SaucerMesh0")); // Create a component
	SaucerMesh->AttachTo(RootComponent); // Attach the component to root
	SaucerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);	// Disable collision on the visible mesh (Use the dedicated collision mesh only)
	
	// Load the Asteroid static mesh asset:
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SaucerMeshAsset(TEXT("StaticMesh'/Game/Art/Models/Saucer.Saucer'"));
	if (SaucerMeshAsset.Succeeded()) { // and if successful,
		SaucerMesh->SetStaticMesh(SaucerMeshAsset.Object);	// Set the mesh as the object
	}

	// Set the starting velocity
	CurrentVelocity = GetStartingVelocity();

	// Cache sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> SaucerHitAudio(TEXT("SoundWave'/Game/Audio/SaucerDestruction.SaucerDestruction'"));
	SaucerHit = SaucerHitAudio.Object;
	
	static ConstructorHelpers::FObjectFinder<USoundBase> SaucerWeaponAudio(TEXT("SoundWave'/Game/Audio/SaucerWeapon.SaucerWeapon'"));
	SaucerFire = SaucerWeaponAudio.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> HitAudio(TEXT("SoundWave'/Game/Audio/ShipHit.ShipHit'"));
	HitSound = HitAudio.Object;

	// Set the initial life span = infinite
	this->InitialLifeSpan = 0.0f; // Is this required for a saucer????

	// Enable firing
	bCanFire = 1;

	// Set the point value
	PointValue = 200;

	// Set up Destruction particles:
	SaucerDestructionParticles = CreateDefaultSubobject<UParticleSystem>(TEXT("TheSaucerDestructionParticles"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem>SaucerDestructionParticleAsset(TEXT("ParticleSystem'/Game/Art/Particles/PlayerShiprDestructionParticles.PlayerShiprDestructionParticles'"));
	if (SaucerDestructionParticleAsset.Succeeded()) {
		SaucerDestructionParticles = SaucerDestructionParticleAsset.Object;
	}
}

// Called when the game starts or when spawned
void ASaucer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASaucer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// Stay close to the player ship
	FollowPlayerShip();

	// Dodge nearby obstacles
	DodgeNearby();
	
	// Cap the velocity
	CurrentVelocity = CurrentVelocity.GetClampedToSize(0.0f, MAX_SAUCER_SPEED);
	CurrentVelocity.Z = 0.0f; // Restrict to the Z plane

	FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
	
	// Apply X Screen Wrap:
	if (NewLocation.X < ScreenXMin)
		NewLocation.X = ScreenXMax;
	else if (NewLocation.X > ScreenXMax)
		NewLocation.X = ScreenXMin;

	// Apply Y Screen Wrap:
	if (NewLocation.Y < ScreenYMin)
		NewLocation.Y = ScreenYMax;
	else if (NewLocation.Y > ScreenYMax)
		NewLocation.Y = ScreenYMin;

	// Set the adjusted position, and rotate the saucer to face the direction it is traveling in:
	this->SetActorLocationAndRotation(NewLocation, CurrentVelocity.Rotation());

	// Fire, if possible
	FireShot();
}

void ASaucer::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

	// Try and play the hit sound effect (if specified)
	if (SaucerHit != NULL) {
		UGameplayStatics::PlaySoundAtLocation(this, SaucerHit, GetActorLocation());
	}

	// Update the game mode
	ASpaceRocksGameMode *TheGameMode = Cast<ASpaceRocksGameMode>(UGameplayStatics::GetGameMode(this)); // Get the game mode
	if (TheGameMode != NULL) {
		AAsteroidSpawner* TheAsteroidSpawner = TheGameMode->GetAsteroidSpawner();
		if (TheAsteroidSpawner != NULL)
			TheAsteroidSpawner->SaucerDestroyed();
		TheGameMode->AddPoints(PointValue);
	}

	// Play the hit sound effect (if specified)
	if (HitSound != NULL) {
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}

	// Fire hit particles
	UWorld* World = AActor::GetWorld(); // Get the World
	if (World != NULL && SaucerDestructionParticles != NULL) {
		UGameplayStatics::SpawnEmitterAtLocation(World, SaucerDestructionParticles, this->GetActorLocation(), this->GetActorRotation(), true);
	}

	// Destroy this saucer
	Destroy();
}

FVector ASaucer::GetStartingVelocity() {
	FRotator StartRotation(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f);
	FVector StartVelocity = StartRotation.Vector() * MAX_SAUCER_SPEED;
	return StartVelocity;
}

void ASaucer::FireShot() {
	if (bCanFire) {
		const FRotator FireRotation = this->GetRandomShotRotation(); // Get a random shot angle

		 // Spawn projectile at an offset from this pawn
		const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(WeaponOffset);

		UWorld* const World = GetWorld();
		if (World != NULL) {
			// spawn the projectile Actor
			World->SpawnActor<ASRSaucerProjectile>(SpawnLocation, FireRotation);
		}
		// Set a timer to toggle bCanFire
		World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &ASaucer::ShotTimerExpired, FIRE_RATE);

		// Disable firing (for now...)
		this->SetCanFire(false);

		// Try and play the sound (if specified)
		if (SaucerFire != NULL) {
			UGameplayStatics::PlaySoundAtLocation(this, SaucerFire, GetActorLocation());
		}
	}
}

// Handler for the fire timer expiry
void ASaucer::ShotTimerExpired() {
	bCanFire = true;
}

// Updates CurrentVelocity to follow the player ship, if neccessary. Called during tick
void ASaucer::FollowPlayerShip() {
	ASpaceRocksGameMode *TheGameMode = Cast<ASpaceRocksGameMode>(UGameplayStatics::GetGameMode(this)); // Get the game mode
	if (TheGameMode) {
		APlayerShip* ThePlayerShip = TheGameMode->GetPlayerShip(); // Get the player ship
		if (ThePlayerShip != NULL && ThePlayerShip->IsValidLowLevel() && !ThePlayerShip->IsPendingKill()) { // Check if the player ship is valid
			// Get the player ship location:
			FVector PlayerLocation(0, 0, 0);
			PlayerLocation = ThePlayerShip->GetActorLocation();

			// Calculate the distance between the saucer and the ship:
			FVector ShipSaucerDistance = PlayerLocation - this->GetActorLocation(); // Find difference between player location and saucer

			// Adjust CurrentVelocity, if neccessary:
			if (ShipSaucerDistance.Size() > MIN_IDEAL_DISTANCE_TO_PLAYER) {
				// Convert ship to saucer distance into a unit vector
				//float OutLength = 0.0f;
				ShipSaucerDistance.ToDirectionAndLength(ShipSaucerDistance, (*new float));

				CurrentVelocity += (ShipSaucerDistance * SAUCER_ACCELERATION); // Accelerate towards player ship
			}
		}
	} // End game mode null check
}

// Dodge nearby obstacles (Asteroids, player ship)
void ASaucer::DodgeNearby() {
	// Find the closest asteroid
	
	ASpaceRocksGameMode *TheGameMode = Cast<ASpaceRocksGameMode>(UGameplayStatics::GetGameMode(this)); // Get the game mode
	if (TheGameMode) {
		APlayerShip* ThePlayerShip = TheGameMode->GetPlayerShip(); // Get the player ship
		AAsteroidSpawner* TheAsteroidSpawner = TheGameMode->GetAsteroidSpawner(); // Get the asteroid spawner
		if (TheAsteroidSpawner) {
			TArray<AAsteroid*>* TheAsteroids = TheAsteroidSpawner->GetAsteroids(); // Get the Asteroid array
			if (TheAsteroids != NULL && TheAsteroids->Num() > 0) {
				// Use the top asteroid as the basis for comparisons
				AAsteroid* ClosestAsteroid = TheAsteroids->Top();
				float ClosestAsteroidDistance = (this->GetActorLocation() - ClosestAsteroid->GetActorLocation()).Size();
				// Compare asteroids
				for (AAsteroid* CurrentAsteroid : *TheAsteroids) { // Check each Asteroid in the array
					float CheckDistance = 10000; // Initialize with a value much larger than the possible play area range
					if (CurrentAsteroid != NULL) {
						CheckDistance = (this->GetActorLocation() - CurrentAsteroid->GetActorLocation()).Size();
						// Check around screen wrap:
						// Top:
						if (this->GetActorLocation().Y > (ScreenYMax - MAX_DISTANCE_TO_ASTEROID)) {
							float WrapCheckDistance = ( FVector(this->GetActorLocation().X, this->GetActorLocation().Y + ScreenYMin, 0.0f) - CurrentAsteroid->GetActorLocation() ).Size();
							if (WrapCheckDistance < CheckDistance)
								CheckDistance = WrapCheckDistance;
						}
						// Bottom:
						else if (this->GetActorLocation().Y < (ScreenYMin + MAX_DISTANCE_TO_ASTEROID)) {
							float WrapCheckDistance = (FVector(this->GetActorLocation().X, this->GetActorLocation().Y + ScreenYMax, 0.0f) - CurrentAsteroid->GetActorLocation()).Size();
							if (WrapCheckDistance < CheckDistance)
								CheckDistance = WrapCheckDistance;
						}
						//Right:
						if (this->GetActorLocation().X > (ScreenXMax - MAX_DISTANCE_TO_ASTEROID)) {
							float WrapCheckDistance = (FVector(this->GetActorLocation().X + ScreenXMin, this->GetActorLocation().Y, 0.0f) - CurrentAsteroid->GetActorLocation()).Size();
							if (WrapCheckDistance < CheckDistance)
								CheckDistance = WrapCheckDistance;
						}
						// Left:
						else if (this->GetActorLocation().X < (ScreenXMin + MAX_DISTANCE_TO_ASTEROID)) {
							float WrapCheckDistance = (FVector(this->GetActorLocation().X + ScreenXMax, this->GetActorLocation().Y, 0.0f) - CurrentAsteroid->GetActorLocation()).Size();
							if (WrapCheckDistance < CheckDistance)
								CheckDistance = WrapCheckDistance;
						}
					}

					if (CheckDistance < ClosestAsteroidDistance) {
						ClosestAsteroid = CurrentAsteroid;
						ClosestAsteroidDistance = CheckDistance;
					}
				}

				// Steer away from the closest asteroid or player ship, if neccessary:
				FVector CourseCorrection(0, 0, 0);
				if ((this->GetActorLocation() - ClosestAsteroid->GetActorLocation()).Size() > MAX_DISTANCE_TO_ASTEROID) { // We must be avoiding an asteroid wrapped around the screen
					CourseCorrection = ClosestAsteroid->GetActorLocation() - this->GetActorLocation();
				}
				else // We're avoiding an asteroid that is not wrapped around the screen
					CourseCorrection = this->GetActorLocation() - ClosestAsteroid->GetActorLocation();

				if ((ThePlayerShip != NULL && ThePlayerShip->IsValidLowLevel() && !ThePlayerShip->IsPendingKill() && (ThePlayerShip->GetActorLocation() - this->GetActorLocation()).Size() < CourseCorrection.Size())) {
					CourseCorrection = ThePlayerShip->GetActorLocation();
					float OutLength = 0.0f;
					CourseCorrection.ToDirectionAndLength(CourseCorrection, OutLength);
					CurrentVelocity += (CourseCorrection * SAUCER_ACCELERATION); // Accelerate away from player ship
				}

				else if (CourseCorrection.Size() < MAX_DISTANCE_TO_ASTEROID) {
					// Convert saucer distance to a unit vector only
					float OutLength = 0.0f;
					CourseCorrection.ToDirectionAndLength(CourseCorrection, OutLength);

					CurrentVelocity += (CourseCorrection * SAUCER_ACCELERATION); // Accelerate away from asteroid
				}
			}
		}
	} // end game mode check
}

// Get a random shot rotation
FRotator ASaucer::GetRandomShotRotation() {
	return FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f);
}