// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUp_Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APowerUp_Actor::APowerUp_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetupAttachment(RootComponent);
	

	RespawnTime = 10.0f;
}


void APowerUp_Actor::Interact_Implementation(APawn* InstigatorPawn)
{
}

void APowerUp_Actor::SetPowerUpState(bool bNewState)
{
	SetActorEnableCollision(bNewState);

	RootComponent->SetVisibility(bNewState, true);
}

void APowerUp_Actor::ShowPowerUp()
{
	SetPowerUpState(true);
}

void APowerUp_Actor::HideAndCoolDown()
{
	SetPowerUpState(false);

	GetWorldTimerManager().SetTimer(TimeHandler_RespawnTime, this, &APowerUp_Actor::ShowPowerUp, RespawnTime);
}



