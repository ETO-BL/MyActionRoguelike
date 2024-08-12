// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"

void ASPlayerController::SetPawn(APawn* NewPawn)
{
	Super::SetPawn(NewPawn);

	OnPawnChanged.Broadcast(NewPawn);
}
