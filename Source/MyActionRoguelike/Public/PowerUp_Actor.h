// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"
#include "PowerUp_Actor.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class MYACTIONROGUELIKE_API APowerUp_Actor : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerUp_Actor();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	virtual void Interact_Implementation(APawn* InstigatorPawn);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
