// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction_Effect.h"
#include "SActionComponent.h"
#include "GameFramework/GameStateBase.h"

USAction_Effect::USAction_Effect()
{
	Duration = 4.f;
	Period = 2.f;

	bAutoStart = true;
}
//Burnning
void USAction_Effect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	//状态持续时间
	if (Duration > 0.f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "StopAction", Instigator);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Duration, Delegate, Duration, false);
	}

	//伤害间隔
	if (Period > 0.f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "ExcutePeriodEffect", Instigator);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Period, Delegate, Period, true);

	}
}

void USAction_Effect::StopAction_Implementation(AActor* Instigator)
{
	if (GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_Period) < KINDA_SMALL_NUMBER)
	{
		ExcutePeriodEffect(Instigator);
	}

	Super::StopAction_Implementation(Instigator);

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Period);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Duration);

	//记得去掉Tag
	USActionComponent* Comp = GetOwningComponent();
	Comp->RemoveAction(this);
	

}

float USAction_Effect::GetTimerRemaining() const
{
	AGameStateBase* GS =  GetWorld()->GetGameState<AGameStateBase>();
	if (GS)
	{
		float Endtime = TimeStarted + Duration;
		//					不同步
		return Endtime - GS->GetServerWorldTimeSeconds();
	}
	return Duration;
}

void USAction_Effect::ExcutePeriodEffect_Implementation(AActor* Instigator)
{
}
