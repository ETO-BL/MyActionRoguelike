// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUp_HealthPotion.h"
#include "SAttributeComponent.h"
#include "Components/StaticMeshComponent.h"

ASPowerUp_HealthPotion::ASPowerUp_HealthPotion()
{

	//�����Ѿ���ʼ��MeshComp,�����ٴδ���
	RootComponent = MeshComp;
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void ASPowerUp_HealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (ensure(InstigatorPawn))
	{
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
		if (ensure(AttributeComp) && !AttributeComp->IsFullHealth())
		{
			HideAndCoolDown();
		}
	}
}
