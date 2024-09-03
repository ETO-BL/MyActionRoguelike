// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayFunctionLibrary.h"
#include "ActionSystem/SAttributeComponent.h"

bool UGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttribute(TargetActor);
	if (AttributeComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyDamage"));
		return AttributeComp->ApplyHealthChanged(DamageCauser, DamageAmount);
	}
	return false;
}

//��Minionʩ����
bool UGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult)
{
	if (ApplyDamage(DamageCauser, TargetActor, DamageAmount))
	{
		//�������������
		UPrimitiveComponent* HitComp = HitResult.GetComponent();
		if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{			
			FVector Direction = HitResult.TraceEnd - HitResult.TraceStart;
			Direction.Normalize();

			//����ʩ�����ķ���
			HitComp->AddImpulseAtLocation(Direction * 300000.0f, HitResult.ImpactPoint, HitResult.BoneName);
			//���е�ķ��߷���
			//HitComp->AddImpulseAtLocation(-HitResult.ImpactNormal * 300000.0f, HitResult.ImpactPoint, HitResult.BoneName);
		}
		return true;
	}
	return false;
}


