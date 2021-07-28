// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceRocks.h"			// Contains global screen boundary values
#include "SRProjectile.h"			// Required for SRProjectile collision/hit handling
#include "SRSaucerProjectile.h"		// Required for SRSaucerProjectile collision/hit handling
#include "SpaceRocksGameMode.h"		// The Space Rocks GameMode
#include "AsteroidLarge.h"		// Required to spawn large asteroids
#include "AsteroidMedium.h"		// Required to spawn medium asteroids
#include "AsteroidSmall.h"		// Required to spawn small asteroids
#include "PlayerShip.h"
#include "Kismet/GameplayStatics.h"	// Required to retrieve GameMode
#include "Asteroid.h"
#include "GameFramework/Actor.h"


// Constructor: Sets default values
AAsteroid::AAsteroid()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set up collision:
	AsteroidCollision = CreateDefaultSubobject<USphereComponent>( TEXT("RootComponent") );	// Create a collision component (To use as the root)
	RootComponent = AsteroidCollision;														// Set the root as the collision component
	AsteroidCollision->InitSphereRadius(100.0f);											// The size of the collision mesh sphere
	
	// Collision:
	// ECollisionChannel:: Collision profile settings:
	// ECollisionChannel::ECC_GameTraceChannel1 = SRProjectile
	// ECollisionChannel::ECC_GameTraceChannel2 = PlayerShip
	// ECollisionChannel::ECC_GameTraceChannel3 = PowerUps
	//**************************************************************
	AsteroidCollision->OnComponentHit.AddDynamic(this, &AAsteroid::OnHit);					// Set up a notification for when this component hits something.
	AsteroidCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	AsteroidCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore); // Disable collision with player ship
	AsteroidCollision->SetNotifyRigidBodyCollision(true);											// Same as "Simulation Generates Hit Events" checkbox
	AsteroidCollision->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics, true);	// Make sure collision is enabled

	// Set this Asteroid's speed
	ASpaceRocksGameMode *TheGameMode = Cast<ASpaceRocksGameMode>(UGameplayStatics::GetGameMode(this)); // Get the Game Mode
	if (TheGameMode != NULL) {
		AAsteroidSpawner *TheAsteroidSpawner = TheGameMode->GetAsteroidSpawner();
		if (TheAsteroidSpawner != NULL) {
			MinAsteroidSpeed = TheAsteroidSpawner->GetMinAsteroidSpeed(this);
			MaxAsteroidSpeed = TheAsteroidSpawner->GetMaxAsteroidSpeed(this);
		}
	}

	// Use a ProjectileMovementComponent to move the Asteroid collision and (therefore the) root components
	AsteroidMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	AsteroidMovement->UpdatedComponent = AsteroidCollision;
	
	AsteroidMovement->InitialSpeed = GetStartingSpeed();
	AsteroidMovement->MaxSpeed = MaxAsteroidSpeed;
		
	// Set the bounciness of the ProjectileMovementComponent:
	AsteroidMovement->bShouldBounce = true;
	AsteroidMovement->Bounciness = 1.0f; // Don't lose any momentum on bounce!

	// Restrict Asteroid movement to the Z plane:	
	AsteroidMovement->bConstrainToPlane = true;
	AsteroidMovement->SetPlaneConstraintNormal( FVector(0.0f, 0.0f, 1.0f) );
	AsteroidMovement->SetPlaneConstraintOrigin( FVector(0.0f, 0.0f, 0.0f) );
	AsteroidMovement->ProjectileGravityScale = 0.0f; // No gravity

	// Create an Asteroid mesh component:
	AsteroidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AsteroidMesh0")); // Create a component
	AsteroidMesh->AttachTo(RootComponent); // Attach the component to root
	AsteroidMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);	// Disable collision on the visible mesh (Use the dedicated collision mesh only)
	
	// Set the color modifier value to a default value other than NULL
	AsteroidColorModifier = 100.0f;	// Note: This value is replaced when Large Asteroids spawn. This is a default failsafe only.

	// Set the initial life span = infinite
	this->InitialLifeSpan = 0.0f;
		
	// Get a random rotation rate:
	AsteroidRotationRate = GetRandomRotationValue();
	AsteroidRotationDirection = GetRandomRotationDirection();

	// Set up particles:
	AsteroidParticles = CreateDefaultSubobject<UParticleSystem>(TEXT("TheAsteroidParticles"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem>AsteroidParticleAsset(TEXT("ParticleSystem'/Game/Art/Particles/AsteroidLargeDestructionParticles.AsteroidLargeDestructionParticles'"));
	if (AsteroidParticleAsset.Succeeded()) {
		AsteroidParticles = AsteroidParticleAsset.Object;
	}
}

// Called when the game starts or when spawned
void AAsteroid::BeginPlay(){
	Super::BeginPlay();


	// Dynamic material setup:
	// Generate a random color modifier value:
	if (this->IsA(AAsteroidLarge::StaticClass())) { // Large Asteroid's only: color is passed to shatters
		AsteroidColorModifier = GetRandomColorModifierValue();
	}
	

	// Create and set a dynamic material instance
	DynamicAsteroidMaterial = UMaterialInstanceDynamic::Create(AsteroidMaterial, this);
	
	// Set the randomized color value:
	DynamicAsteroidMaterial->SetScalarParameterValue(FName(TEXT("AsteroidColorModifier")), AsteroidColorModifier);

	// Assign the material to the mesh:
	AsteroidMesh->SetMaterial(0, DynamicAsteroidMaterial);
}

// Called every frame
void AAsteroid::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// Screen wrap the asteroid: X
	if (GetActorLocation().X < ScreenXMin)
		SetActorLocation(*(new FVector(ScreenXMax, GetActorLocation().Y, 0.0f)));
	else if (GetActorLocation().X > ScreenXMax)
		SetActorLocation(*(new FVector(ScreenXMin, GetActorLocation().Y, 0.0f)));

	// Screen wrap the asteroid: Y
	if (GetActorLocation().Y < ScreenYMin)
		SetActorLocation(*(new FVector(GetActorLocation().X, ScreenYMax, 0.0f)));
	else if (GetActorLocation().Y > ScreenYMax)
		SetActorLocation(*(new FVector(GetActorLocation().X, ScreenYMin, 0.0f)));

	// Rotate the Asteroid:
	FRotator AsteroidRotation = AsteroidMesh->GetComponentRotation();	// Get the Asteroid mesh's current rotation
	switch (AsteroidRotationDirection) {
	case EAsteroidRotationDirection::ERoll:
		// Primary rotation:
		AsteroidRotation.Roll += AsteroidRotationRate;	// Increase it by this Asteroid's rotation speed
		// Add some wonkiness:
		AsteroidRotation.Yaw += AsteroidRotationRate/ ASTEROID_WONKINESS;	// Increase it by this Asteroid's rotation speed
		break;

	case EAsteroidRotationDirection::EYaw:
		// Primary rotation:
		AsteroidRotation.Yaw += AsteroidRotationRate;	// Increase it by this Asteroid's rotation speed
		// Add some wonkiness:
		AsteroidRotation.Pitch += AsteroidRotationRate / ASTEROID_WONKINESS;	// Increase it by this Asteroid's rotation speed
		break;
	default: break;
	}

	// Set the new calculated rotation
	AsteroidMesh->SetWorldRotation(AsteroidRotation);
}

// Handle collision hits
void AAsteroid::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

	// Collision handling:
	if (OtherActor != NULL && OtherComp != NULL) {
		// Handle Asteroid-on-Asteroid and Asteroid-on-PlayerShip collisions:
		if (OtherActor->IsA(AAsteroid::StaticClass() ) || OtherActor->IsA(APlayerShip::StaticClass())) {
			// Randomize the asteroid's rotation and speed when it is hit
			AsteroidRotationRate = GetRandomRotationValue();
			AsteroidRotationDirection = GetRandomRotationDirection();

			// Handle shield hits: Increase velocity!
			if (OtherActor != NULL && OtherActor->IsA(APlayerShip::StaticClass())) {
				APlayerShip* ThePlayerShip = Cast<APlayerShip>(OtherActor);
				if (OtherComp == ThePlayerShip->GetShieldMesh()) {
					AsteroidMovement->MaxSpeed = AsteroidMovement->MaxSpeed *= AsteroidShieldBounceMultiplier;
					AsteroidMovement->SetVelocityInLocalSpace(AsteroidMovement->Velocity * AsteroidShieldBounceMultiplier);
				}
					
			}

			// TO-DO: Spray particles etc !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}

		// Handle projectile and player ship hits:
		if ((OtherActor->IsA(ASRProjectile::StaticClass()) )) {
			// Get the location and rotation of the current asteroid being hit
			FVector SpawnLocation = this->GetActorLocation();
			FRotator SpawnRotation = this->GetActorRotation();

			// Add points
			AddPoints();
			
			// Fire hit particles
			UWorld* World = AActor::GetWorld(); // Get the World
			if (World != NULL && AsteroidParticles != NULL) {
				UGameplayStatics::SpawnEmitterAtLocation(World, AsteroidParticles, this->GetActorLocation(), this->GetActorRotation(), true);
			}

			if (TheAsteroidSpawner != NULL) {
				// Spawn new, smaller Asteroid "shatters":
				TheAsteroidSpawner->SpawnShatter(SpawnLocation, SpawnRotation, this);

				// Destroy the current Asteroid and update the asteroid count:
				TheAsteroidSpawner->AsteroidDestroyed(this); // De-register & destroy the Asteroid from the spawner
			}
			
			// Get a new rotation rate:
			AsteroidRotationRate = GetRandomRotationValue();
		} // End projectile and player ship hit handling

	} // End NULL test
	else {
		UE_LOG(LogTemp, Warning, TEXT("AAsteroid::OnHit() Error! Something is NULL"));
	}
}

// Generate a random starting speed for an Asteroid
// Returns a float between min and max asteroid speed
// Is overridden in Medium and Small Asteroids
float AAsteroid::GetStartingSpeed() {
	return FMath::RandRange(MinAsteroidSpeed, MaxAsteroidSpeed);
}

// Add points, based on the size of the Asteroid being destroyed:
void AAsteroid::AddPoints() {
	
	// Register points with the Asteroid Spawner:
	if (TheAsteroidSpawner != NULL) {
		if (this->IsA(AAsteroidLarge::StaticClass())) {			// Add points for a LARGE asteroid
				TheAsteroidSpawner->AddPoints(LARGE_ASTEROID_POINTS);
		}
		else if (this->IsA(AAsteroidMedium::StaticClass())) {	// Add points for a MEDIUM asteroid
			TheAsteroidSpawner->AddPoints(MEDIUM_ASTEROID_POINTS);
		}
		else if (this->IsA(AAsteroidSmall::StaticClass())) {	// Add points for a SMALL asteroid
			TheAsteroidSpawner->AddPoints(SMALL_ASTEROID_POINTS);
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("AAsteroid::AddPoints() Error! TheAsteroidSpawner is NULL"));
}

// Record the Asteroid's spawner object:
void AAsteroid::RegisterParentSpawner(AAsteroidSpawner* ParentSpawner) {
	// Record this Asteroid's Spawner
	if (ParentSpawner != NULL) {
		TheAsteroidSpawner = ParentSpawner;
	} else {
		UE_LOG(LogTemp, Warning, TEXT("WARNING! AAsteroid::RegisterParentSpawner ParentSpawner is NULL!"));
	}
}

// Returns a float between min and max Asteroid scale
float AAsteroid::GetRandomScaleValue() {
	return FMath::RandRange(MIN_ASTEROID_SCALE, MAX_ASTEROID_SCALE);
}

// Returns a float between min and max Asteroid rotation
float AAsteroid::GetRandomRotationValue() {
	return FMath::RandRange(MIN_ASTEROID_ROTATION, MAX_ASTEROID_ROTATION);
}

// Returns a float between min and max Asteroid saturation
float AAsteroid::GetRandomColorModifierValue() {
	return FMath::RandRange(MIN_ASTEROID_COLOR_MODIFIER, MAX_ASTEROID_COLOR_MODIFIER);	// Otherwise, return another negative value
}

// Set the Asteroid's color, and update's the dynamic material
void AAsteroid::SetColorModifier(float NewColorModifier) {
	AsteroidColorModifier = NewColorModifier; // Set the incoming value

	// Update the material:
	if (DynamicAsteroidMaterial != NULL)
		DynamicAsteroidMaterial->SetScalarParameterValue(FName(TEXT("AsteroidColorModifier")), AsteroidColorModifier);
}

// Get the Asteroid's randomly generated color modifier (ie. to give it to a shatter)
float AAsteroid::GetColorModifier() {
	return AsteroidColorModifier;
}

// Get a random EAsteroidRotationDirection
EAsteroidRotationDirection AAsteroid::GetRandomRotationDirection() {
	int RotationSelector = FMath::RandRange(0, 1);
	if(RotationSelector == 0)
		return EAsteroidRotationDirection::EYaw; // Spins clockwise/counterclockwise relative to game's camera
	else 
		return EAsteroidRotationDirection::ERoll;
}

//Useful:
//LogTemp:Warning: Asteroid BEING constructed!CurrentVelocity = x : 0.000000, y : 0.000000, z : 0.000000, rotation = -14.535398
//LogTemp : Warning : this->GetActorLocation().X : -727.164551, Y : 237.060181, Z : 0.000000
//LogTemp : Warning : AsteroidMesh->GetComponentLocation().X : -727.164551, Y : 237.060181, Z : 0.000000
//LogTemp : Warning : AsteroidMesh->GetComponentRotation().pitch : 0.000000, yaw : -14.535398, roll : 76.752815


//Useless:
//LogTemp : Warning : this->GetActorRotation().pitch : 0.000000, yaw : 0.000000, roll : 0.000000

/*
UE_LOG(LogTemp, Warning, TEXT("Asteroid BEING constructed! CurrentVelocity = x: %f, y: %f, z: %f, rotation = %f"), CurrentVelocity.X, CurrentVelocity.Y, CurrentVelocity.Z, this->GetActorRotation().Yaw);
UE_LOG(LogTemp, Warning, TEXT("this->GetActorLocation(). X: %f, Y: %f, Z: %f"), this->GetActorLocation().X, this->GetActorLocation().Y, this->GetActorLocation().Z);
UE_LOG(LogTemp, Warning, TEXT("this->GetActorRotation(). pitch: %f, yaw: %f, roll: %f"), this->GetActorRotation().Pitch, this->GetActorRotation().Pitch, this->GetActorRotation().Pitch);
UE_LOG(LogTemp, Warning, TEXT("AsteroidMesh->GetComponentLocation(). X: %f, Y: %f, Z: %f"), AsteroidMesh->GetComponentLocation().X, AsteroidMesh->GetComponentLocation().Y, AsteroidMesh->GetComponentLocation().Z);
UE_LOG(LogTemp, Warning, TEXT("AsteroidMesh->GetComponentRotation(). pitch: %f, yaw: %f, roll: %f"), AsteroidMesh->GetComponentRotation().Pitch, AsteroidMesh->GetComponentRotation().Yaw, AsteroidMesh->GetComponentRotation().Roll);
*/