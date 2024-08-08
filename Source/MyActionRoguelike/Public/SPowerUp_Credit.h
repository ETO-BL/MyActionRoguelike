// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerUp_Actor.h"
#include "SPowerUp_Credit.generated.h"

/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API ASPowerUp_Credit : public APowerUp_Actor
{
	GENERATED_BODY()
	
public:

	virtual void Interact_Implementation(APawn* InstigatorPawn)override;

	ASPowerUp_Credit();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "CreditAdd")
	float CreditsToAdd;


};
