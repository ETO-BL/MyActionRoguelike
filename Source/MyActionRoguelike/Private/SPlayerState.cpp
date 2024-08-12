// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"

ASPlayerState::ASPlayerState()
{
	Credits = 100.0f;

	//  PlayerState默认进行网络复制
	//	SetIsReplicatedByDefault(true);
}


void ASPlayerState::OnRe_Credits(float OldCredits)
{
	OnCreditsChanged.Broadcast(this, Credits, Credits - OldCredits);
}

float ASPlayerState::GetCredits()
{
	return Credits;
}

void ASPlayerState::AddCredits(float Delta)
{
	if (!ensure(Delta > 0.f))
	{
		return ;
	}
	Credits += Delta;

	OnCreditsChanged.Broadcast(this, Credits, Delta);
}

bool ASPlayerState::DelCredits(float Delta)
{
	if (!ensure(Delta >= 0.0f))
	{
		return false;
	}
	if (Credits < Delta)
	{
		return false;
	}
	Credits -= Delta;
	OnCreditsChanged.Broadcast(this, Credits, -Delta);
	UE_LOG(LogTemp, Warning, TEXT("Credits: %f"), &Credits);
	return true;
}


void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 添加 Credits 属性到复制列表
	DOREPLIFETIME(ASPlayerState, Credits);
}
