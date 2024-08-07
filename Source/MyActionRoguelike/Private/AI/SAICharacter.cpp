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

	TimeToHitParamName = "TimeToHit";
	TargetActorKey = "TargetActor";
}
 
void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnSeePawn);
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChange);
}

void ASAICharacter::OnHealthChange(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	//受伤
	if (Delta < 0.0f)
	{
		if (InstigatorActor != this)
		{
			SetTargetActor(InstigatorActor);
		}

		UE_LOG(LogTemp, Warning, TEXT("Flash"));
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

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

void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIC = GetController<AAIController>();
	//获取TargetActor
	AIC->GetBlackboardComponent()->SetValueAsObject(TargetActorKey, NewTarget);
}

AActor* ASAICharacter::GetTargetActor() const
{
	AAIController* AIC = GetController<AAIController>();
	return Cast<AActor>(AIC->GetBlackboardComponent()->GetValueAsObject(TargetActorKey));
}

void ASAICharacter::OnSeePawn(APawn* Pawn)
{
	// Ignore if target already set
	if (GetTargetActor() != Pawn)
	{
		SetTargetActor(Pawn);

		
	}
	DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 0.5f, true);
}



