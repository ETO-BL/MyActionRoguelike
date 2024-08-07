// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMagicProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;
class USoundCue;
class UAudioComponent;

UCLASS()
class MYACTIONROGUELIKE_API ASMagicProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASMagicProjectile();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UAudioComponent> AudioComp2;

	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UProjectileMovementComponent> MovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UParticleSystemComponent> EffectComp;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<USoundCue> ImpactSound;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageAmount;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	//virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
