// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTaskNode_HealSelf.h"
#include "SAttributeComponent.h"
#include "AIController.h"


EBTNodeResult::Type USBTTaskNode_HealSelf::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();

	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttribute(AIPawn);

	if (ensure(AttributeComp))
	{
		AttributeComp->ApplyHealthChanged(AIPawn, AttributeComp->GetMaxHealth());
		UE_LOG(LogTemp, Warning, TEXT("Healing"))
	}

	return EBTNodeResult::Succeeded;
}
