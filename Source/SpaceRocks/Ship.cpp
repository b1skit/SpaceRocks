// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceRocks.h"		// Contains global screen boundary values
#include "Ship.h"


// Sets default values
AShip::AShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	//// Set the pawn to be controlled by the lowest numbered player:
	//AutoPossessPlayer = EAutoReceiveInput::Player0; 

	// Create a dummy root component we can attach things to.
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")); // Mandatory
	
	// Moved to child:
	/*
	// Create a visible object
	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	
	// Set up collision detection
	ShipMesh->BodyInstance.SetCollisionProfileName("Pawn");			// Set the Collision profile name
	ShipMesh->OnComponentHit.AddDynamic(this, &AShip::OnHit);		// Set up a notification for when this component hits something
	ShipMesh->SetNotifyRigidBodyCollision(true);					// Same as "Simulation Generates Hit Events" checkbox
	ShipMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	// Attach the ShipMesh to the RootComponent
	ShipMesh->AttachTo(RootComponent);	// Mandatory
	*/
	


	UE_LOG(LogTemp, Warning, TEXT("Ship created!"));
}

// Called when the game starts or when spawned
void AShip::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AShip::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

/*
// Handle collision hits
void AShip::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

	UE_LOG(LogTemp, Warning, TEXT("Ship hit detected!") );

	//Destroy(); // Breaks things!
}
*/