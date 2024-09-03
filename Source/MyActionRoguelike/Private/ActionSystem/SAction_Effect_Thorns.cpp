// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/SAction_Effect_Thorns.h"
#include "ActionSystem/SAttributeComponent.h"
#include "ActionSystem/SActionComponent.h"
#include "GameplayFunctionLibrary.h"

USAction_Effect_Thorns::USAction_Effect_Thorns()
{
	ReflectFraction = 0.2f;

	Duration = 0.f;
	Period = 0.f;
}

void USAction_Effect_Thorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	USAttributeComponent* Attribute = USAttributeComponent::GetAttribute(GetOwningComponent()->GetOwner());
	if (Attribute)
	{
		Attribute->OnHealthChanged.AddDynamic(this, &USAction_Effect_Thorns::OnHealthChange);
	}

}

void USAction_Effect_Thorns::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	USAttributeComponent* Attribute = USAttributeComponent::GetAttribute(GetOwningComponent()->GetOwner());
	if (Attribute)
	{
		Attribute->OnHealthChanged.RemoveDynamic(this, &USAction_Effect_Thorns::OnHealthChange);
	}
}

void USAction_Effect_Thorns::OnHealthChange(AActor* InstigatorActor, USAttributeComponent* AttributeComp, float NewHealth, float Delta)
{
	AActor* OwningActor = GetOwningComponent()->GetOwner();

	if (Delta < 0.f && OwningActor != InstigatorActor)
	{
		float ReflectDamage = FMath::Abs(Delta * ReflectFraction);

		UGameplayFunctionLibrary::ApplyDamage(OwningActor, InstigatorActor, ReflectDamage);
	}
}
