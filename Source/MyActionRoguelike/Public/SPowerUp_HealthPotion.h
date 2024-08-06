// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerUp_Actor.h"
#include "SPowerUp_HealthPotion.generated.h"

/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API ASPowerUp_HealthPotion : public APowerUp_Actor
{
	GENERATED_BODY()

public:

	ASPowerUp_HealthPotion();
protected:

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	
};
