// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerUP/PowerUp_Actor.h"
#include "SPowerUp_Action.generated.h"

class USAction;

/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API ASPowerUp_Action : public APowerUp_Actor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	TSubclassOf<USAction> ActionToGrant;

public:

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
};
