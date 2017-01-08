// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "GameFramework/Actor.h"
#include "SRProjectile.generated.h"

class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS(config = Game)
class ASRProjectile : public AActor
{
	GENERATED_BODY()
public:
	// Constructor
	ASRProjectile(); 

	// Tick: Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Handle the projectile hitting something
	UFUNCTION()
	void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	// Returns ProjectileMovement subobject
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	// Projectile collision
	USphereComponent *SRProjectileCollision;

	// Get the collision object
	USphereComponent* GetSRProjectileCollision();

private:
	// Projectile variables:
	const float ProjectileInitialSpeed = 2700.0f;
	const float ProjectileMaxSpeed = 2700.0f;
	const float SRProjectileLifeSpan = 0.41f;	// Initial lifespawn for a projectile	

	// Projectile movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	// Projectile sounds
	USoundBase *ImpactSound;

	// Projectile particles
	UParticleSystemComponent *ProjectileTrail;
};

FORCEINLINE USphereComponent* ASRProjectile::GetSRProjectileCollision() {
	return SRProjectileCollision;
}