// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUp_Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

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
	

	RespawnTime = 5.0f;

	//bIsAlive = true;
	SetReplicates(true);
}

//客户端自动调用
void APowerUp_Actor::OnRep_IsAlive()
{
	UE_LOG(LogTemp, Warning, TEXT("bIsAlive: %s"), bIsAlive ? TEXT("true") : TEXT("false"));

	SetActorEnableCollision(bIsAlive);

	RootComponent->SetVisibility(bIsAlive, true);
}


void APowerUp_Actor::Interact_Implementation(APawn* InstigatorPawn)
{
}

void APowerUp_Actor::SetPowerUpState(bool bNewState)
{
	bIsAlive = bNewState;
	//主动调用同步到服务器
	OnRep_IsAlive();
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

//更新复制属性到服务器
void APowerUp_Actor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APowerUp_Actor, bIsAlive);
}

