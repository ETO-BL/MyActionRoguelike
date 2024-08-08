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


bool USAttributeComponent::ApplyHealthChanged(AActor* Instigator, float Delta)
{
	if (!GetOwner()->CanBeDamaged())
	{
		return false;
	}

	float OldHealth = Health;

	Health = FMath::Clamp(Health += Delta, 0, MaxHealth);

	//���ܳ���Ѫ������,������Ҫ����ʵ�ʱ仯Ѫ��
	float ActualDela = Health - OldHealth;
	OnHealthChanged.Broadcast(Instigator, this, Health, ActualDela);

	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);
	return ActualDela != 0;
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

float USAttributeComponent::GetMaxHealth()
{
	return MaxHealth;
}

float USAttributeComponent::GetHealth()
{
	return Health;
}



USAttributeComponent* USAttributeComponent::GetAttribute(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}

	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* Actor)
{
	USAttributeComponent* AttributeComp = GetAttribute(Actor);

	if (AttributeComp)
	{
		return AttributeComp->IsAlive();
	}

	return false;
}

bool USAttributeComponent::Kill(AActor* Instigator)
{
	return ApplyHealthChanged(Instigator, -GetMaxHealth());
}