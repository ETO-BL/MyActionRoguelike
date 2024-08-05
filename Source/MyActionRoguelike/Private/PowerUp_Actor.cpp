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

}

// Called when the game starts or when spawned
void APowerUp_Actor::BeginPlay()
{
	Super::BeginPlay();
	
}

void APowerUp_Actor::Interact_Implementation(APawn* InstigatorPawn)
{
}

// Called every frame
void APowerUp_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

