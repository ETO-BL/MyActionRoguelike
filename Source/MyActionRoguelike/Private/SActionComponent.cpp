// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"
#include "SAction.h"


USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}


void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		for (TSubclassOf<USAction> ActionClass : DefaultActions)
		{
			AddAction(GetOwner(), ActionClass);
		}
	}
}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//显示当前角色Tag
	FString DebugMessage = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, DebugMessage);
}

void USActionComponent::AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}

	//实例化一个对象
	USAction* NewAction = NewObject<USAction>(this, ActionClass);
	if (NewAction)
	{
		Actions.Add(NewAction);

		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}
	}
 }

bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* Action : Actions)
	{
		//找到要执行的技能
		if (Action && Action->ActionName == ActionName)
		{
			if (!Action->CanStart(Instigator))
			{
				FString DebugMessage = FString::Printf(TEXT("Failed to run : %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, DebugMessage);
				continue;
			}

			Action->StartAction(Instigator);
			return true;
		}
	}

	return false;
}

bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (Action->IsRunning())
			{
				Action->StopAction(Instigator);
				return true;
			}			
		}
	}

	return false;
}

void USActionComponent::RemoveAction(USAction* Action)
{
	if (!ensure(Action && Action->IsRunning()))
	{
		return;
	}
	Actions.Remove(Action);
}

