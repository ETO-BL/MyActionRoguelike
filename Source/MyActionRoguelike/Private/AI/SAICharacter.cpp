// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SAttributeComponent.h"
#include "BrainComponent.h"
#include "SWorldUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SActionComponent.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("Attributecomp");
	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	//Hit Mesh Only
	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	TimeToHitParamName = "TimeToHit";
	TargetActorKey = "TargetActor";
	
	FadeOutDuration = 5.f;
	PawnSensingComp->SightRadius = 1500.f;
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
		
		//如果还没创建血条
		if (ActiveHealthBar == nullptr)
		{
			//创建Minion血条
			ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if (ActiveHealthBar)
			{
				//记得构造时获取Attribute绑定到HealthBar
				ActiveHealthBar->AttachedActor = this;
				ActiveHealthBar->AddToViewport();
			}
		}
		

		//应用HitFlash
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

			//死亡效果
			GetWorld()->GetTimerManager().SetTimer(FadeOutTimerHandle, this, &ASAICharacter::FadeOutUpdate, 0.1f, true);
			FadeOutStartTime = GetWorld()->TimeSeconds;

			//来点夸张的效果-用于施加死亡后实体受力
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();

			//Setlifespan
			SetLifeSpan(5.f);
		}
	}
}

void ASAICharacter::FadeOutUpdate()
{
	float ElapsedTime = GetWorld()->GetTimeSeconds() - FadeOutStartTime;
	float Alpha = FMath::Clamp(ElapsedTime / FadeOutDuration, 0.0f, 1.0f);

	// Update FadeOut parameter
	UpdateMaterialFadeOut(Alpha);

	if (Alpha >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(FadeOutTimerHandle);
	}
}

void ASAICharacter::UpdateMaterialFadeOut(float Alpha)
{
	if (GetMesh())
	{
		UMaterialInstanceDynamic* DynMaterial = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
		if (DynMaterial)
		{
			DynMaterial->SetScalarParameterValue("FadeOut", Alpha);
		}
	}
}

void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIC = GetController<AAIController>();
	//获取TargetActor
	if (ensureAlways(NewTarget))
	{
		AIC->GetBlackboardComponent()->SetValueAsObject(TargetActorKey, NewTarget);
	}	
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

		//通知其他客户端绘制被发现的UI
		MulticastPawnSeen();
	}
	//DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 0.5f, true);
}

//通知其他客户端绘制被发现的UI
void ASAICharacter::MulticastPawnSeen_Implementation()
{
	SopttedWidget = CreateWidget<USWorldUserWidget>(GetWorld(), SpottedWidgetClass);
	if (SopttedWidget)
	{
		SopttedWidget->AttachedActor = this;
		SopttedWidget->AddToViewport(10);
	}
}



