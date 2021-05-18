// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PowerUp.generated.h"

UCLASS()
class SPACEROCKS_API APowerUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerUp();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Handle PowerUp destruction clean-up
	virtual void Destroyed();

	// Handle the PowerUp being collected
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UStaticMeshComponent* PowerUpMesh;
	UMaterial* PowerUpMaterial;
private:
	float PowerUpVelocity = 100.0f;				// The rate the power up is moving
	const float POWERUP_LIFESPAN = 20.0f;		// How long before the powerup disappears
	FVector PowerUpDirection;					// The direction the power up is moving in
	
	USphereComponent* PowerUpCollision;			// The Powerup collision sphere component

	// Get a random travel direction for the powerup
	FVector GetPowerUpDirection();

	// PowerUp sound
	USoundBase *CollectSound;					// Sound to play when the powerup is collected
};


