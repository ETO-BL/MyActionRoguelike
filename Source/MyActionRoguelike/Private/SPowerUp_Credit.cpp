// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUp_Credit.h"
#include "SPlayerState.h"


ASPowerUp_Credit::ASPowerUp_Credit()
{
	CreditsToAdd = 50.f;
}

void ASPowerUp_Credit::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn))
	{
		return;
	}

	ASPlayerState* PlayerState = InstigatorPawn->GetController()->GetPlayerState<ASPlayerState>();
	if (ensure(PlayerState))
	{
		PlayerState->AddCredits(CreditsToAdd);
		HideAndCoolDown();
	}
}


