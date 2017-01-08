// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceRocks.h"
#include "PlayerShip.h"
#include "SRProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

ASRProjectile::ASRProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Collision:
	// ECollisionChannel:: Collision profile settings:
	// ECollisionChannel::ECC_GameTraceChannel1 = SRProjectile
	// ECollisionChannel::ECC_GameTraceChannel2 = PlayerShip
	// ECollisionChannel::ECC_GameTraceChannel3 = PowerUps
	//**************************************************************
	SRProjectileCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SRProjectileCollision;
	SRProjectileCollision->InitSphereRadius(10.0f);

	// Collision settings:
	SRProjectileCollision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	SRProjectileCollision->OnComponentHit.AddDynamic(this, &ASRProjectile::OnHit);		// set up a notification for when this component hits something
	SRProjectileCollision->SetNotifyRigidBodyCollision(true);
	SRProjectileCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	SRProjectileCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECR_Ignore); // Disable collision with player ship
	SRProjectileCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Ignore); // Disable collision with other projectiles
	SRProjectileCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECR_Ignore); // Disable collision with powerups

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = SRProjectileCollision;
	ProjectileMovement->InitialSpeed = ProjectileInitialSpeed;
	ProjectileMovement->MaxSpeed = ProjectileMaxSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f; // No gravity

	// Set the initial lifespan of the projectile
	this->InitialLifeSpan = SRProjectileLifeSpan;
	
	// Cache sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> ImpactAudio(TEXT("SoundWave'/Game/Audio/ProjectileHit.ProjectileHit'"));
	ImpactSound = ImpactAudio.Object;

	// Set up Projectile particles:
	ProjectileTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileTrailParticles"));
	
	ProjectileTrail->AttachTo(SRProjectileCollision);
	ProjectileTrail->bAutoActivate = true;
	ProjectileTrail->SetRelativeLocation(FVector::ZeroVector);

	static ConstructorHelpers::FObjectFinder<UParticleSystem>ProjectileParticlesAsset(TEXT("ParticleSystem'/Game/Art/Particles/ProjectileTrailParticles.ProjectileTrailParticles'"));
	if (ProjectileParticlesAsset.Succeeded()) {
		ProjectileTrail->SetTemplate(ProjectileParticlesAsset.Object);
		
	}
}

// Called every frame
void ASRProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Apply X Screen Wrap:
	if (this->GetActorLocation().X < ScreenXMin) {
		this->SetActorLocation(*(new FVector(ScreenXMax, this->GetActorLocation().Y, 0.0f)));
	}
	else if (this->GetActorLocation().X > ScreenXMax) {
		this->SetActorLocation(*(new FVector(ScreenXMin, this->GetActorLocation().Y, 0.0f)));
	}
		

	// Apply Y Screen Wrap:
	if (this->GetActorLocation().Y < ScreenYMin) {
		this->SetActorLocation(*(new FVector(this->GetActorLocation().X, (float)ScreenYMax, 0.0f)));
	}
		
	else if (this->GetActorLocation().Y > ScreenYMax) {
		this->SetActorLocation(*(new FVector(this->GetActorLocation().X, (float)ScreenYMin, 0.0f)));
	}
}

void ASRProjectile::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){
	// Only add impulse and destroy projectile if we hit a physics object
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) )
	{	
		// Try and play the sound (if specified)
		if (ImpactSound != NULL) {
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		}

		Destroy();
	}
	
}



