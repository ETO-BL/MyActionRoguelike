// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUp_HealthPotion.h"
#include "SAttributeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SPlayerState.h"

ASPowerUp_HealthPotion::ASPowerUp_HealthPotion()
{
	CreditsDelta = 20.f;
}

void ASPowerUp_HealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn))
	{
		return;
	}
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttribute(InstigatorPawn);

	if (ensure(AttributeComp) && !AttributeComp->IsFullHealth())
	{
		ASPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();	
		if (PlayerState)
		{
			//检查分数够不够
			if (PlayerState->DelCredits(CreditsDelta) && AttributeComp->ApplyHealthChanged(this, AttributeComp->GetMaxHealth()))
			{
				HideAndCoolDown();
				UE_LOG(LogTemp, Warning, TEXT("Hide"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No PlayerState!"));
		}
	}
}
