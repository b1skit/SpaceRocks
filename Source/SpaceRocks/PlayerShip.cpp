// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceRocks.h"		// Contains global screen boundary values
#include "SpaceRocksGameMode.h"		// Needed to send death notification
#include "SRProjectile.h"			// Ship's weapon projectiles
#include "PowerUp.h"				// PowerUps
#include "PowerUpShield.h"			// Shield Powerup
#include "PowerUpLife.h"			// Extra life powerup
#include "PowerUpPoints.h"			// Extra points powerup
#include "PowerUpWeapon.h"			// Weapon powerup
#include "PlayerShip.h"

const FName APlayerShip::PlayerShipThrusterPitch("PlayerShipThrusterPitch");
const FName APlayerShip::PlayerShipThrusterVolume("PlayerShipThrusterVolume");

APlayerShip::APlayerShip() {
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set the PlayerShip to be controlled by the lowest numbered player:
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Cache sound effects
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("SoundWave'/Game/Audio/LaserFire.LaserFire'"));
	FireSound = FireAudio.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> HitAudio(TEXT("SoundWave'/Game/Audio/ShipHit.ShipHit'"));
	HitSound = HitAudio.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> SpawnAudio(TEXT("SoundWave'/Game/Audio/SpawnSound.SpawnSound'"));
	SpawnSound = SpawnAudio.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> OneUpSoundAudio(TEXT("SoundWave'/Game/Audio/OneUp.OneUp'"));
	OneUpSound = OneUpSoundAudio.Object;

	// Player Ship Thruster Sounds:
	static ConstructorHelpers::FObjectFinder<USoundCue>PlayerShipSoundCue(TEXT("SoundCue'/Game/Audio/PlayerShipThrusterCue.PlayerShipThrusterCue'"));
	PlayerShipThrusterSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PlayerShipThrusterSound"));
	if (PlayerShipSoundCue.Succeeded()) {
		PlayerShipThrusterSoundComponent->SetSound(PlayerShipSoundCue.Object);
		PlayerShipThrusterSoundComponent->AttachTo(ShipMesh);
	}

	// Set the collision object:
	ShipCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent")); 
	RootComponent = ShipCollision;
	ShipCollision->InitSphereRadius(50.0f);
	
	// Collision:
	// ECollisionChannel:: Collision profile settings:
	// ECollisionChannel::ECC_GameTraceChannel1 = SRProjectile
	// ECollisionChannel::ECC_GameTraceChannel2 = PlayerShip
	// ECollisionChannel::ECC_GameTraceChannel3 = PowerUps
	//**************************************************************
	ShipCollision->OnComponentHit.AddDynamic(this, &APlayerShip::OnHit); 
	ShipCollision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	ShipCollision->SetNotifyRigidBodyCollision(true);
	ShipCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block); // MUST be set each round in the constructor
	ShipCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Ignore); // Ignore the SRprojectile channel
	
	ShipCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECR_Block);
	ShipCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Configure Ship mesh:
	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh0"));
	ShipMesh->AttachTo(RootComponent);

	// Set meshes:
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMeshAsset(TEXT("/Game/Art/Models/playerShip.playerShip"));
	if (ShipMeshAsset.Succeeded()) { // and if successful, use it to get a mesh reference
		ShipMesh->SetStaticMesh(ShipMeshAsset.Object);	// Set the FObjectFinder static mesh reference
	}
	// Set material:
	static ConstructorHelpers::FObjectFinder<UMaterial>PlayerShipMaterialInstance(TEXT("Material'/Game/Art/Materials/ShipMaterial.ShipMaterial'")); // Get a reference to an asset in the content browser
	if (PlayerShipMaterialInstance.Succeeded())
		PlayerShipMaterial = (UMaterial*)PlayerShipMaterialInstance.Object;		// Set the material

	// Disable visible static mesh auto-collision
	ShipMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Mid Thruster:
	MidThrusterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MidThrusterMesh"));
	MidThrusterMesh->AttachTo(ShipMesh);
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MidThrusterMeshAsset(TEXT("StaticMesh'/Game/Art/Models/PlayerShipThrusters.PlayerShipThrusters_MidThruster'"));
	if (MidThrusterMeshAsset.Succeeded()) {
		MidThrusterMesh->SetStaticMesh(MidThrusterMeshAsset.Object);
	}
	
	// Left Thruster:
	LeftThrusterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftThrusterMesh"));
	LeftThrusterMesh->AttachTo(ShipMesh);
	static ConstructorHelpers::FObjectFinder<UStaticMesh>LeftThrusterMeshAsset(TEXT("StaticMesh'/Game/Art/Models/PlayerShipThrusters.PlayerShipThrusters_LeftThruster'"));
	if (LeftThrusterMeshAsset.Succeeded()) { 
		LeftThrusterMesh->SetStaticMesh(LeftThrusterMeshAsset.Object);
	}

	// Right Thruster:
	RightThrusterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightThrusterMesh"));
	RightThrusterMesh->AttachTo(ShipMesh);
	static ConstructorHelpers::FObjectFinder<UStaticMesh>RightThrusterMeshAsset(TEXT("StaticMesh'/Game/Art/Models/PlayerShipThrusters.PlayerShipThrusters_RightThruster'"));
	if (RightThrusterMeshAsset.Succeeded()) {
		RightThrusterMesh->SetStaticMesh(RightThrusterMeshAsset.Object);
	}
	
	// Set material for all thrusters:
	static ConstructorHelpers::FObjectFinder<UMaterial>ThrusterdMaterialInstance(TEXT("Material'/Game/Art/Materials/PlayerShipThrusterAnimatedMaterial.PlayerShipThrusterAnimatedMaterial'"));
	if (ThrusterdMaterialInstance.Succeeded()) {
		ThrusterMaterial = (UMaterial*)ThrusterdMaterialInstance.Object;		// Set the material

		UE_LOG(LogTemp, Warning, TEXT("ThrusterdMaterialInstance Succeeded!"));
	}
	
	// Weapon
	bCanFire = true;

	// Weapon type and special weapon configs
	NumSpreadShots = 1; // Initialize the spreadshots to 1


	// Shield
	ShieldValue = MaxShieldValue;
	bShieldActivated = false;

	// Configure Shield mesh:
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->AttachTo(ShipMesh);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShieldMeshAsset(TEXT("StaticMesh'/Game/Art/Models/PlayerShipShieldMesh.PlayerShipShieldMesh'"));
	if (ShieldMeshAsset.Succeeded()) {
		ShieldMesh->SetStaticMesh(ShieldMeshAsset.Object);
	}
	
	// Configure shield collision:
	ShieldMesh->OnComponentHit.AddDynamic(this, &APlayerShip::OnShieldHit);
	ShieldMesh->SetNotifyRigidBodyCollision(true); // Enable 'Simulation Generates Hit Events'
	ShieldMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block); // MUST be set each round in the constructor
	ShieldMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Ignore); // Ignore the SRprojectile channel

	bDamageable = true;

	// Set shield material:
	static ConstructorHelpers::FObjectFinder<UMaterial>PlayerShipShieldMaterialInstance(TEXT("Material'/Game/Art/Materials/ShieldMaterial.ShieldMaterial'")); // Get a reference to an asset in the content browser
	if (PlayerShipShieldMaterialInstance.Succeeded())
		PlayerShipShieldMaterial = (UMaterial*)PlayerShipShieldMaterialInstance.Object;		// Set the material

	// Set up Destruction particles:
	PlayerShipDestructionParticles = CreateDefaultSubobject<UParticleSystem>(TEXT("ThePlayerShipDestructionParticles"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem>PlayerShipDestructionParticleAsset(TEXT("ParticleSystem'/Game/Art/Particles/PlayerShiprDestructionParticles.PlayerShiprDestructionParticles'"));
	if (PlayerShipDestructionParticleAsset.Succeeded()) {
		PlayerShipDestructionParticles = PlayerShipDestructionParticleAsset.Object;
	}
}

// Called when the game starts or when spawned
void APlayerShip::BeginPlay(){
	Super::BeginPlay();

	// Register the ship with the game mode (for Saucer AI)
	ASpaceRocksGameMode *TheGameMode = Cast<ASpaceRocksGameMode>(UGameplayStatics::GetGameMode(this));
	TheGameMode->RegisterPlayerShip(this);

	// Set a timer to toggle to "can fire" flag
	UWorld* const World = GetWorld();
	if (World != NULL) {
		// Set the player to be invincible for 3 seconds:
		bDamageable = false;	// Turn damageability off
		ShipCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Turn off collision
		// Set a timer to reverse the player back to damageable in INVINCIBILITY_TIME seconds
		World->GetTimerManager().SetTimer(TimerHandle_SpawnInvincibility, this, &APlayerShip::InvincibilityExpired, InvincibilityTime);

		// Flash while not damageable:
		World->GetTimerManager().SetTimer(TimerHandle_InvincibilityFlash, this, &APlayerShip::ToggleInvincibilityFlash, InvincibilityFlashFrequency);
	}
	
	// Try and play the (re)sound (if specified)
	if (SpawnSound != NULL) {
		UGameplayStatics::PlaySoundAtLocation(this, SpawnSound, GetActorLocation());
	}

	// Assign materials:
	if (ShipMesh != NULL)
		ShipMesh->SetMaterial(0, PlayerShipMaterial);
	if (MidThrusterMesh != NULL)
		MidThrusterMesh->SetMaterial(0, ThrusterMaterial);
	if (LeftThrusterMesh != NULL)
		LeftThrusterMesh->SetMaterial(0, ThrusterMaterial);
	if (RightThrusterMesh != NULL)
		RightThrusterMesh->SetMaterial(0, ThrusterMaterial);
	



	// THIS DOESN'T SEEM TO WORK PROPERLY???
	if (ShipMesh != NULL)
		ShipMesh->SetWorldScale3D(FVector(SizeScale, SizeScale, SizeScale) ); // Scale the ship mesh size
	//UE_LOG(LogTemp, Warning, TEXT("Ship RelativeScale3D: x: %f y:%f z:%f"), ShipMesh->RelativeScale3D.X, ShipMesh->RelativeScale3D.Y, ShipMesh->RelativeScale3D.Z);



	// Start the thruster sound 
	PlayerShipThrusterSoundComponent->Play();

	// Sheild:

	if (ShieldMesh != NULL)
		ShieldMesh->SetVisibility(false);

	if (ShieldMesh != NULL)
		ShieldMesh->SetMaterial(0, PlayerShipShieldMaterial);

	ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Turn off collision
}

// Bind functionality to input:
void APlayerShip::SetupPlayerInputComponent(class UInputComponent* InputComponent){
	Super::SetupPlayerInputComponent(InputComponent);

	// Fire the weapon
	InputComponent->BindAction("FireWeapon", IE_Pressed, this, &APlayerShip::FireShot);	

	// Activate the shield
	InputComponent->BindAction("ActivateShield", IE_Pressed, this, &APlayerShip::ActivateShield);
	InputComponent->BindAction("ActivateShield", IE_Released, this, &APlayerShip::DeactivateShield);

	// Events: Respond to X and Y Axis inputs -> Move the ship
	InputComponent->BindAxis("Thrust", this, &APlayerShip::Move_XAxis);
	InputComponent->BindAxis("RotateShip", this, &APlayerShip::Move_YAxis);
}

// TICK: Called every frame
void APlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Apply drag:
	CurrentVelocity *= ShipDrag;

	// Decrease remaining thruster burn times
	if(MidThrusterBurnOutRemaining > 0)
		MidThrusterBurnOutRemaining -= ThrusterFadeRate;
	if (RightThrusterBurnOutRemaining > 0)
		RightThrusterBurnOutRemaining -= ThrusterFadeRate;
	if (LeftThrusterBurnOutRemaining > 0)
		LeftThrusterBurnOutRemaining -= ThrusterFadeRate;

	// Turn the MID thruster on/off
	if (MidThrusterBurnOutRemaining <= 0) {
		MidThrusterMesh->SetVisibility(false, false);
	}
	else { // Scale the MID thruster
		MidThrusterMesh->SetRelativeScale3D(FVector(MidThrusterBurnOutRemaining, MidThrusterBurnOutRemaining, 1.0f));
	}

	// Turn the LEFT thruster on/off
	if (LeftThrusterBurnOutRemaining <= 0) {
		LeftThrusterMesh->SetVisibility(false, false);
	}
	else { // Scale the LEFT thruster
		LeftThrusterMesh->SetRelativeScale3D(FVector(LeftThrusterBurnOutRemaining, LeftThrusterBurnOutRemaining, 1.0f));
	}

	// Turn the RIGHT thruster on/off
	if (RightThrusterBurnOutRemaining <= 0) {
		RightThrusterMesh->SetVisibility(false, false);
	}
	else { // Scale the RIGHT thruster
		RightThrusterMesh->SetRelativeScale3D(FVector(RightThrusterBurnOutRemaining, RightThrusterBurnOutRemaining, 1.0f));
	}

	// Thruster Sound: Adjust the thruster pitch and volume based on the current thruster states
	float ThrusterValue = FMath::Max(MidThrusterBurnOutRemaining, FMath::Max(LeftThrusterBurnOutRemaining, RightThrusterBurnOutRemaining));
	PlayerShipThrusterSoundComponent->SetFloatParameter(PlayerShipThrusterPitch, ThrusterValue);
	PlayerShipThrusterSoundComponent->SetFloatParameter(PlayerShipThrusterVolume, ThrusterValue);

	// Handle movement based on MoveX/"MoveY" axes, wrap ship around the screen if neccessary
	if (!CurrentVelocity.IsZero())
	{
		// Calculate new location for this tick:
		FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
			
		// Apply X Screen Wrap:
		if (NewLocation.X < ScreenXMin)
			NewLocation.X = ScreenXMax;
		else if(NewLocation.X > ScreenXMax)
			NewLocation.X = ScreenXMin;

		// Apply Y Screen Wrap:
		if (NewLocation.Y < ScreenYMin)
			NewLocation.Y = ScreenYMax;
		else if (NewLocation.Y > ScreenYMax)
			NewLocation.Y = ScreenYMin;

		// Set the adjusted position:
		this->SetActorLocation(NewLocation, true);
	}

	// Correct ship roll (toward center)
	FRotator ShipRotation = this->GetActorRotation();					// Get the current rotation value
	if (ShipRotation.Roll < 0) {
		ShipRotation.Roll = (int)ShipRotation.Roll;
		ShipRotation.Roll++;
	}
	else if (ShipRotation.Roll > 0) {
		ShipRotation.Roll = (int)ShipRotation.Roll;
		ShipRotation.Roll--;
	}
	this->SetActorRotation(ShipRotation);								// Assign the new rotation to the ship

	// Burn the shield, if it's active
	if (bShieldActivated) {
		ShieldValue -= ShieldBurnRate * DeltaTime;
	}
} // End Tick


// X-Axis: Thrust
void APlayerShip::Move_XAxis(float AxisValue){

	// Add forward direction thrust to the current ship velocity value:
	if (AxisValue > 0) {			// Acceleration
		CurrentVelocity += this->GetActorForwardVector() * AxisValue * ShipAcceleration;

		if (MidThrusterBurnOutRemaining < ThrusterBurnOutTime) {
			MidThrusterBurnOutRemaining += ThrusterGrowthRate;
		}
		
		MidThrusterMesh->SetVisibility(true, false);		// Turn thruster on
	}
	else if (AxisValue < 0) {	// Braking
		CurrentVelocity += this->GetActorForwardVector() * AxisValue * ShipBraking;

		// Apply (additional) thruster fade out (also applied in Tick)
		if(MidThrusterBurnOutRemaining > 0)
			MidThrusterBurnOutRemaining -= ThrusterFadeRate;
	}
}

// Y-Axis: Rotate the ship
void APlayerShip::Move_YAxis(float AxisValue)
{
	// Addjust the ship's yaw (rotate around Z-Axis) based on the incoming axis input value
	FRotator ShipRotation = this->GetActorRotation();					// Get the current rotation value
	ShipRotation.Yaw += FMath::Clamp(AxisValue, -1.0f, 1.0f) * RotationSpeed;	// Adjust the value

	// Roll the ship and adjust the thrusters
	if (AxisValue < 0 ) {		
		if (ShipRotation.Roll > -MaxRoll) // Roll left
			ShipRotation.Roll += AxisValue * RollSpeed;

		RightThrusterBurnOutRemaining = ThrusterBurnOutTime; // Reset burnout time
		RightThrusterMesh->SetVisibility(true, false); // Turn thruster on

		// Apply (additional) LEFT thruster fade out (also applied in Tick)
		if (LeftThrusterBurnOutRemaining > 0) {
			LeftThrusterBurnOutRemaining -= ThrusterFadeRate;
		}
	}
	else if (AxisValue > 0) {	// Roll right
		if(ShipRotation.Roll < MaxRoll)
			ShipRotation.Roll += AxisValue * RollSpeed;

		LeftThrusterBurnOutRemaining = ThrusterBurnOutTime; // Reset burnout time
		LeftThrusterMesh->SetVisibility(true, false); // Turn thruster on

		// Apply (additional) LEFT thruster fade out (also applied in Tick)
		if (RightThrusterBurnOutRemaining > 0) {
			RightThrusterBurnOutRemaining -= ThrusterFadeRate;
		}
	}

	// Assign the new rotation to the ship
	this->SetActorRotation(ShipRotation);						
}


// SHIP WEAPONS:
// Fire a shot:
void APlayerShip::FireShot(){
	
		// Fire, if we're able to:
		UWorld* const World = GetWorld();
		if (bCanFire && World != NULL) {
			const FRotator FireRotation = this->GetActorRotation(); // Get the angle to shoot from based on ship

			// Calculate weapon positions
			float WeaponBreadth = 200;
			float WeaponOffset = WeaponBreadth / NumSpreadShots; // Calculate the first position, then offset from there
			float CurrentPosition = (WeaponBreadth / 2) - (WeaponOffset / 2); // Calculate the first position across the breadth of the weapon

			float StaggerPosition = GunForwardOffset; // Initialize the stagger position
			
			// Loop, spawning NumSpreadShots projectiles at calculated offsets
			for (int i = 0; i < NumSpreadShots; i++) {
				// Spawn a projectile at the currently calculated position
				World->SpawnActor<ASRProjectile>(GetActorLocation() + FireRotation.RotateVector(FVector(i % 2 == 0 ? GunForwardOffset : GunForwardOffset + GunForwardStagger, CurrentPosition, 0.f)), FireRotation);

				// Deincrement the weapon position
				CurrentPosition -= WeaponOffset;
			
			} // end firing for loop

			// Set a timer to toggle the "can fire" flag
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &APlayerShip::ShotTimerExpired, FireRate);
		
			// Disable firing (for now...)
			bCanFire = false;
		
			// Try and play the sound (if specified)
			if (FireSound != NULL){
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}
		} // End if World
}

// Allow the player to fire again:
void APlayerShip::ShotTimerExpired(){
	bCanFire = true;
}

// Handle direct ship collision hits
void APlayerShip::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	// Handle pickup collection 
	if (OtherActor != NULL && OtherActor->IsInA (APowerUp::StaticClass())) { // TO DO: Check if OtherActor INHERITS from APowerUp !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
		// Handle Shield power ups
		if (OtherActor->IsA(APowerUpShield::StaticClass())) {
			APowerUpShield* ThePowerUp = Cast <APowerUpShield>(OtherActor); // Cast the pointer
			// Increment the shield value
			ShieldValue += ThePowerUp->GetPowerUpValue();
			if (ShieldValue > MaxShieldValue) // Clamp the shield value
				ShieldValue = MaxShieldValue;
		}

		// Handle extra life powerups
		else if (OtherActor->IsA(APowerUpLife::StaticClass())) {
			// Get the game mode
			ASpaceRocksGameMode *TheGameMode = Cast<ASpaceRocksGameMode>(UGameplayStatics::GetGameMode(this));
			if (TheGameMode != NULL)
				TheGameMode->AddLife();
			
			// Play the one up sound
			this->PlayOneUpSound(); // One up!
		}

		// Handle Points powerup
		else if (OtherActor->IsA(APowerUpPoints::StaticClass())) {
			// Get the game mode
			ASpaceRocksGameMode *TheGameMode = Cast<ASpaceRocksGameMode>(UGameplayStatics::GetGameMode(this));
			if (TheGameMode != NULL) {
				APowerUpPoints* ThePowerUp = Cast <APowerUpPoints>(OtherActor);
				
				TheGameMode->AddPoints(ThePowerUp->GetPointValue());
			}		
		}

		// Handle Weapon powerups
		else if (OtherActor->IsA(APowerUpWeapon::StaticClass()) && NumSpreadShots < MAX_SPREAD_SHOTS) {
			NumSpreadShots++;
		}
		
	} // End powerup handling

	// Handle damage from projectiles or asteroids
	else if (bDamageable && OtherActor != NULL && OtherActor != this && (!bShieldActivated || (bShieldActivated && ShieldValue <= 0))) {
		
		// Lower the shield value to 0
		ShieldValue = 0;

		// Get the game mode:
		ASpaceRocksGameMode *TheGameMode = Cast<ASpaceRocksGameMode>(UGameplayStatics::GetGameMode(this));

		// Update the game mode about the player's death:
		if (TheGameMode != NULL)
			TheGameMode->PlayerDied();

		// Play the hit sound effect (if specified)
		if (HitSound != NULL) {
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		}

		// Fire hit particles
		UWorld* World = AActor::GetWorld(); // Get the World
		if (World != NULL && PlayerShipDestructionParticles != NULL) {
			UGameplayStatics::SpawnEmitterAtLocation(World, PlayerShipDestructionParticles, this->GetActorLocation(), this->GetActorRotation(), true);
		}

		// Destroy the player ship
		Destroy();
	}

}

// Handle shield hits
void APlayerShip::OnShieldHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	// Handle Shield deflections
	if (bDamageable && OtherActor != NULL && OtherActor != this && !OtherActor->IsA( APowerUp::StaticClass() ) && bShieldActivated && ShieldValue > 0) {
		ShieldValue -= 25; // TO DO: Alter damage based on impact type??????????????????????????????????????????? HAVE ASTEROIDS SPECIFY THEIR OWN DAMAGE!!!

		if (ShieldValue <= 0) // If the shield is now exhausted, disable it
			DeactivateShield();

		// TO DO: IMPLEMENT SHIELD IMPACT SOUND!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}
}

// Handler for when invincibility expires
void APlayerShip::InvincibilityExpired() {
	bDamageable = true;				// Set the bDamageable boolean back to true
	ShipCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // Turn collision back on
	ShipCollision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
}

// Checks if the player ship is damageable
// Returns bDamageable
uint32 APlayerShip::IsDamageable() {
	return bDamageable;
}

// Toggle visibility on/off
void APlayerShip::ToggleInvincibilityFlash() {
	UWorld* const World = GetWorld(); // Get the world
	if (World != NULL && !bDamageable) {
		if (this->bHidden) {	// If hidden, unhide
			this->SetActorHiddenInGame(false);
		}
		else {					// If unhidden, hide
			this->SetActorHiddenInGame(true);
		} // Start a new timer
		World->GetTimerManager().SetTimer(TimerHandle_InvincibilityFlash, this, &APlayerShip::ToggleInvincibilityFlash, InvincibilityFlashFrequency);
	}
	else {
		this->SetActorHiddenInGame(false); // Set the player visible again
	}
	
}

// Play the 1-Up sound. Triggered by the game mode as appropriate
void APlayerShip::PlayOneUpSound() {
	if (OneUpSound != NULL) {
		UGameplayStatics::PlaySoundAtLocation(this, OneUpSound, GetActorLocation());
	}
}

// Activate the shield
void APlayerShip::ActivateShield() {
	if (ShieldValue > 0) { // Only activate if the shield is not exhausted
		bShieldActivated = true;
		ShieldMesh->SetVisibility(true);
		ShieldMesh->SetHiddenInGame(false);
		ShieldMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // Turn on collision
	}
}

// Deactivate the shield
void APlayerShip::DeactivateShield() {
	bShieldActivated = false;
	ShieldMesh->SetVisibility(false);
	ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Turn off collision
}

// Increase the shield value
void APlayerShip::IncreaseShieldValue(uint8 AddValue) {
	ShieldValue += AddValue;
}