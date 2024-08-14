// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"
#include "SActionComponent.h"
#include "Net/UnrealNetwork.h"


bool USAction::CanStart_Implementation(AActor* Instigator)
{
	if (IsRunning())
	{
		return false;
	}

	USActionComponent* Comp = GetOwningComponent();

	if (Comp->ActiveGameplayTags.HasAny(BlockTags))
	{
		return false;
	}

	return true;
}

void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Running: %s"), *GetNameSafe(this));

	//添加技能
	USActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.AppendTags(GrantTags);

	RepData.bIsRunning = true;
	RepData.Instigator = Instigator;

	GetOwningComponent()->OnActionStarted.Broadcast(GetOwningComponent(), this);
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Stoped: %s"), *GetNameSafe(this));

	//ensureAlways(bIsRunning);

	//去掉Tag--机能
	USActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.RemoveTags(GrantTags);

	RepData.bIsRunning = false;
	RepData.Instigator = Instigator;

	GetOwningComponent()->OnActionStoped.Broadcast(GetOwningComponent(), this);
}

UWorld* USAction::GetWorld() const
{
	//Outer 在用NewObject创建action的时候设置的
	UActorComponent* Comp = Cast<UActorComponent>(GetOuter());
	if (Comp)
	{
		return Comp->GetWorld();
	}
	return nullptr;
}

bool USAction::IsSupportedForNetworking() const
{
	return true;
}

USActionComponent* USAction::GetOwningComponent() const
{
	return Cast<USActionComponent>(GetOuter());
}

//同步到服务器
void USAction::OnRep_RepData()
{
	if (RepData.bIsRunning)
	{
		StartAction(RepData.Instigator);
	}
	else
	{
		StopAction(RepData.Instigator);
	}
}

bool USAction::IsRunning()
{
	return RepData.bIsRunning;
}


//更新复制属性
void USAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAction, RepData);
	
}


