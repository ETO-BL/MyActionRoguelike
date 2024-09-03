// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile/SProjectileBase.h"
#include "DashProjectile.generated.h"

/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API ADashProjectile : public ASProjectileBase
{
	GENERATED_BODY()

public:
	ADashProjectile();
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Delay")
	float TeleportDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Delay")
	float DetonateDelay;


	FTimerHandle TimerHandle_DelayDetonate;

	virtual void Explode_Implementation() override;

	void TeleportInstigator();
	
	virtual void BeginPlay() override;


};
