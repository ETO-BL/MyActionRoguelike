// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUP/SPowerUp_Action.h"
#include "ActionSystem/SAction.h"
#include "ActionSystem/SActionComponent.h"

void ASPowerUp_Action::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn && ActionToGrant))
	{
		return;
	}

	USActionComponent* ActionComp = Cast<USActionComponent>(InstigatorPawn->GetComponentByClass(USActionComponent::StaticClass()));
	if (ActionComp)
	{
		ActionComp->AddAction(ActionToGrant, InstigatorPawn);
		HideAndCoolDown();
	}
}
