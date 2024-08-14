// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "Blueprint/UserWidget.h"

void ASPlayerController::TooglePauseMenu()
{
	//�ر�
	if (PauseMenu && PauseMenu->IsInViewport())
	{
		PauseMenu->RemoveFromParent();
		PauseMenu = nullptr;//����GC

		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());

		return;
	}

	//��
	PauseMenu = CreateWidget<UUserWidget>(this, PauseMenuClass);
	if (PauseMenu)
	{
		PauseMenu->AddToViewport(100);


		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());
	}
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("PauseMenu", IE_Pressed, this, &ASPlayerController::TooglePauseMenu);
}

void ASPlayerController::SetPawn(APawn* NewPawn)
{
	Super::SetPawn(NewPawn);

	OnPawnChanged.Broadcast(NewPawn);
}

void ASPlayerController::BeginPlayingState()
{
	BlueprintBeginPlayingState();
}

void ASPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	OnPlayerStateReceived.Broadcast(PlayerState);
}


