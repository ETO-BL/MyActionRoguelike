// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"
#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"


static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("BL.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for AttributeComponent"), ECVF_Cheat);

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	Health = 100;
	MaxHealth = 100;
	Rage = 50;
	MaxRage = 100;

	SetIsReplicatedByDefault(true);
}



bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}


bool USAttributeComponent::ApplyHealthChanged(AActor* Instigator, float Delta)
{
	if (!GetOwner()->CanBeDamaged() && Delta < 0.f)
	{
		return false;
	}

	if (Delta < 0.f)
	{
		float DamageMultiper = CVarDamageMultiplier.GetValueOnGameThread();

		Delta *= DamageMultiper;

	}


	float OldHealth = Health;

	Health = FMath::Clamp(Health + Delta, 0, MaxHealth);

	//可能超出血量上限,所以需要计算实际变化血量
	float ActualDela = Health - OldHealth;
	//OnHealthChanged.Broadcast(Instigator, this, Health, ActualDela);


	if (ActualDela != 0.f)
	{
		MulticastHealthChanged(Instigator, Health, ActualDela);
	}
	

	//死了
	if (ActualDela < 0 && Health <= 0)
	{
		ASGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASGameModeBase>();
		if (GameMode)
		{
			GameMode->KillActor(GetOwner(), Instigator);
		}
	}

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

bool USAttributeComponent::AddRage(AActor* Instigator, float Delta)
{
	float OldRage = Rage;
	Rage = FMath::Clamp(Rage + Delta, 0, MaxRage);

	float ActualDelta = Rage - OldRage;

	OnRageChanged.Broadcast(Instigator, this, Rage, ActualDelta);
	
	return FMath::IsNearlyZero(ActualDelta);
}

float USAttributeComponent::GetRage()
{
	return Rage;
}

float USAttributeComponent::GetMaxRage()
{
	return MaxRage;
}

bool USAttributeComponent::IsFullRage()
{
	return Rage >= MaxRage;
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(USAttributeComponent, Health);
	DOREPLIFETIME(USAttributeComponent, MaxHealth);
	
	//先运行起来再优化
	//仅仅为了优化  只发送必要的数据
	//DOREPLIFETIME_CONDITION(USAttributeComponent, MaxHealth, COND_OwnerOnly);
	//DOREPLIFETIME_CONDITION(USAttributeComponent, MaxHealth, COND_InitialOnly);
}

void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* Instigator, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(Instigator, this, NewHealth, Delta);
}