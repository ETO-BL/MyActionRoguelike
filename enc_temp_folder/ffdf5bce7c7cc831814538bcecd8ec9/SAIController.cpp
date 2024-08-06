// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviorTree);

	////参数 this 表示当前上下文（通常是一个Actor或GameMode）0--玩家索引
	//APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	//if (PlayerPawn)
	//{
	//	GetBlackboardComponent()->SetValueAsVector("MoveToLocation", PlayerPawn->GetActorLocation()); 

	//	GetBlackboardComponent()->SetValueAsObject("TargetActor", PlayerPawn);
	//}
}
