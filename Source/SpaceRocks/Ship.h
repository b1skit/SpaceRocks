// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Ship.generated.h"

UCLASS()
class SPACEROCKS_API AShip : public APawn
{
	GENERATED_BODY()

public:
	// Constructor: Sets default values for this pawn's properties
	AShip();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Handle collision hits
	//UFUNCTION()
	//void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	

private:


};
