// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SAttributeComponent.h"
#include "BrainComponent.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("Attributecomp");

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}
 
void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChange);
}

void ASAICharacter::OnHealthChange(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	//受伤
	if (Delta < 0.0f)
	{


		if (NewHealth <= 0.f)
		{
			//StopBT
			AAIController* AIC = Cast<AAIController>(GetController());

			if (AIC)
			{
				AIC->GetBrainComponent()->StopLogic("Killed");
			}

			//死亡启用物理模拟--ragdoll
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");

			//Setlifespan
			SetLifeSpan(10.f);
		}
	}
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	AAIController* AIC = GetController<AAIController>();
	//获取TargetActor
	AIC->GetBlackboardComponent()->SetValueAsObject("TargetActor", Pawn);
	UE_LOG(LogTemp, Warning, TEXT("PlayerSpotted"))
}



