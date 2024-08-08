// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

ASPlayerState::ASPlayerState()
{
	Credits = 100.0f;
}


float ASPlayerState::GetCredits()
{
	return Credits;
}

void ASPlayerState::AddCredits(float Delta)
{
	if (!ensure(Delta > 0.f))
	{
		return ;
	}
	Credits += Delta;

	OnCreditsChanged.Broadcast(this, Credits, Delta);
}

bool ASPlayerState::DelCredits(float Delta)
{
	if (!ensure(Delta >= 0.0f))
	{
		return false;
	}
	if (Credits < Delta)
	{
		return false;
	}
	Credits -= Delta;
	OnCreditsChanged.Broadcast(this, Credits, -Delta);
	UE_LOG(LogTemp, Warning, TEXT("Credits: %f"), &Credits);
	return true;
}



