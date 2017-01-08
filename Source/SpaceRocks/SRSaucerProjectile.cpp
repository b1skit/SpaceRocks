// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceRocks.h"
#include "SRSaucerProjectile.h"

// Constructor:
ASRSaucerProjectile::ASRSaucerProjectile() {

	// Collision:
	// ECollisionChannel:: Collision profile settings:
	// ECollisionChannel::ECC_GameTraceChannel1 = SRProjectile
	// ECollisionChannel::ECC_GameTraceChannel2 = PlayerShip
	// ECollisionChannel::ECC_GameTraceChannel3 = SRSaucerProjectile
	// ECollisionChannel::ECC_GameTraceChannel4 = Saucer
	//**************************************************************
	this->GetSRProjectileCollision()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
	//SRProjectileCollision->OnComponentHit.AddDynamic(this, &ASRProjectile::OnHit);		// set up a notification for when this component hits something
	this->GetSRProjectileCollision()->SetNotifyRigidBodyCollision(true);
	this->GetSRProjectileCollision()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	this->GetSRProjectileCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECR_Ignore); // Disable collision with saucer
	this->GetSRProjectileCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECR_Ignore); // Disable collision with other projectiles

	
}


