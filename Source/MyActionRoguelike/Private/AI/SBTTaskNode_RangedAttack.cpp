// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTaskNode_RangedAttack.h"
#include "AIController.h"
#include "Gameframework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ActionSystem/SAttributeComponent.h"

USBTTaskNode_RangedAttack::USBTTaskNode_RangedAttack()
{
	MaxBulletSpread = 2.0f;
}

EBTNodeResult::Type USBTTaskNode_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyAIController = OwnerComp.GetAIOwner();

	if (ensure(MyAIController))
	{
		//��ȡAI
		ACharacter* MyPawn = Cast<ACharacter>(MyAIController->GetPawn());
		if (MyPawn == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		//��ȡ��ɫ
		AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));

		if (TargetActor == nullptr)
		{
			return EBTNodeResult::Failed;
		} 

		//�жϽ�ɫ�Ƿ���,�����Ļ��Ͳ��ٹ���,������ǰ��֧
		if (!USAttributeComponent::IsActorAlive(TargetActor))
		{
			return EBTNodeResult::Failed;
		}

		//�����ӵ�����λ�úͷ���
		FVector MuzzleLocation = MyPawn->GetMesh()->GetSocketLocation("Muzzle_01");
		FVector TargetDirection = TargetActor->GetActorLocation() - MuzzleLocation;
		FRotator MuzzleRotation = TargetDirection.Rotation();
		//���ƫ��
		MuzzleRotation.Pitch += FMath::RandRange(0.f, MaxBulletSpread);
		MuzzleRotation.Yaw += FMath::RandRange(-MaxBulletSpread, MaxBulletSpread);

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Instigator = MyPawn;

		//�����ӵ�,����
		AActor* NewProj = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, Params);

		return NewProj ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;

	}

	return EBTNodeResult::Failed;
}



