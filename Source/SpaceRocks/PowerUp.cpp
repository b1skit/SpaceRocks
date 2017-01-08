// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceRocks.h"
#include "PlayerShip.h"
#include "SpaceRocksGameMode.h"		// Needed to send powerup collection notification
#include "PowerUp.h"


// Sets default values
APowerUp::APowerUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Collision:
	// ECollisionChannel:: Collision profile settings:
	// ECollisionChannel::ECC_GameTraceChannel1 = SRProjectile
	// ECollisionChannel::ECC_GameTraceChannel2 = PlayerShip
	// ECollisionChannel::ECC_GameTraceChannel3 = PowerUps
	//**************************************************************
	PowerUpCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));	// Create a collision component (To use as the root)
	RootComponent = PowerUpCollision;													// Set the root as the collision component
	PowerUpCollision->InitSphereRadius(80.0f);											// The size of the collision mesh sphere
	PowerUpCollision->SetCollisionProfileName(FName(TEXT("BlockAllDynamic")));

	PowerUpCollision->OnComponentHit.AddDynamic(this, &APowerUp::OnHit);							// Set up a notification for when this component hits something

	// Collision settings:
	PowerUpCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PowerUpCollision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3); 
	PowerUpCollision->SetNotifyRigidBodyCollision(true);
	PowerUpCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	PowerUpCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECR_Block);
	PowerUpCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Ignore);

	// Create an PowerUp mesh component:
	PowerUpMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PowerUpMesh")); // Create a component
	PowerUpMesh->AttachTo(RootComponent); // Attach the component to root
	PowerUpMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);	// Disable collision on the visible mesh (Use the dedicated collision mesh only)
																				// Assign the PowerUp static mesh:
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PowerUpMeshAsset(TEXT("StaticMesh'/Game/Art/Models/PowerUpPlane.PowerUpPlane'"));
	if (PowerUpMeshAsset.Succeeded()) { // and if successful,
		PowerUpMesh->SetStaticMesh(PowerUpMeshAsset.Object);	// Set the mesh as the object
	}

	// Get a random powerup direction
	PowerUpDirection = GetPowerUpDirection();
	
	// Set the initial life span = infinite
	this->InitialLifeSpan = POWERUP_LIFESPAN; // TO DO: FLASH WHEN ABOUT TO EXPIRE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Cache sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> PowerUpAudio(TEXT("SoundWave'/Game/Audio/CollectPowerUp.CollectPowerUp'"));
	CollectSound = PowerUpAudio.Object;
}

// Called when the game starts or when spawned
void APowerUp::BeginPlay()
{
	Super::BeginPlay();
	
	// Assign materials:
	if (PowerUpMesh != NULL)
		PowerUpMesh->SetMaterial(0, PowerUpMaterial);
}

// Called every frame
void APowerUp::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// Move the PowerUp
	FVector PowerUpLocation = this->GetActorLocation();
	PowerUpLocation += PowerUpDirection * PowerUpVelocity * DeltaTime;
	this->SetActorLocation(PowerUpLocation, true);

	// Screen wrap the PowerUp: X
	if (GetActorLocation().X < ScreenXMin)
		SetActorLocation(*(new FVector(ScreenXMax, GetActorLocation().Y, 0.0f)));
	else if (GetActorLocation().X > ScreenXMax)
		SetActorLocation(*(new FVector(ScreenXMin, GetActorLocation().Y, 0.0f)));

	// Screen wrap the PowerUp: Y
	if (GetActorLocation().Y < ScreenYMin)
		SetActorLocation(*(new FVector(GetActorLocation().X, ScreenYMax, 0.0f)));
	else if (GetActorLocation().Y > ScreenYMax)
		SetActorLocation(*(new FVector(GetActorLocation().X, ScreenYMin, 0.0f)));
}

// Handle PowerUp destruction clean-up
void APowerUp::Destroyed(){
	// Notify the AsteroidSpawner that the PowerUp has been destroyed
	ASpaceRocksGameMode *TheGameMode = Cast<ASpaceRocksGameMode>(UGameplayStatics::GetGameMode(this));
	if (TheGameMode != NULL) {
		AAsteroidSpawner* TheAsteroidSpawner = TheGameMode->GetAsteroidSpawner();
		if (TheAsteroidSpawner != NULL) {
			TheAsteroidSpawner->PowerUpDestroyed();
		}
	}
}

// Handle the PowerUp being collected
void APowerUp::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

	if (OtherActor != NULL && OtherActor->IsA(APlayerShip::StaticClass() ) ){
		// Notify the game mode:
		ASpaceRocksGameMode *TheGameMode = Cast<ASpaceRocksGameMode>(UGameplayStatics::GetGameMode(this));
		if (TheGameMode != NULL) {
			// Play the collection sound:
			if (CollectSound != NULL) {
				UGameplayStatics::PlaySoundAtLocation(this, CollectSound, GetActorLocation());
			}

			Destroy(); // Destroy the powerup when it is collected (Asteroid spawner notified in Destroyed() )
		}
	}
}

// Get a random travel direction for the powerup
FVector APowerUp::GetPowerUpDirection() {
	FVector Direction(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), 0.0f);
	Direction.Normalize();
	return Direction;
}
