// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//����ai Pawn��Target Actor�ľ���

	UBlackboardComponent* BlackComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackComp))
	{
		AActor* TargetActor = Cast<AActor>(BlackComp->GetValueAsObject("TargetActor"));
		if(TargetActor)
		{  
			AAIController* MyAIController = OwnerComp.GetAIOwner();
			if (ensure(MyAIController))
			{
				APawn* AIPawn = MyAIController->GetPawn();
				//�����AI�Ƿ񻹻���
				if (ensure(AIPawn))
				{
					float Distance = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());

					bool bInAttackRange = Distance < 2000.f;

					bool bHasLOS = false;
					if (bInAttackRange)
					{
						bHasLOS = MyAIController->LineOfSightTo(TargetActor);
					}

					BlackComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, (bInAttackRange && bHasLOS));
				}
			}					
		}
	}


}
