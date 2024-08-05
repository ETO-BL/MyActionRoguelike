// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	Health = 100;
	MaxHealth = 100;
}


bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}


bool USAttributeComponent::ApplyHealthChanged(float Delta)
{
	Health += Delta;

	OnHealthChanged.Broadcast(nullptr, this, Health, Delta);

	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);
	return true;
}

USAttributeComponent* USAttributeComponent::GetAttributeComponent(AActor* FromActor)
{
	if (FromActor)
	{
		return FromActor->FindComponentByClass<USAttributeComponent>();
	}

	return nullptr;
}

bool USAttributeComponent::IsFullHealth()
{
	return Health >= MaxHealth;
}

