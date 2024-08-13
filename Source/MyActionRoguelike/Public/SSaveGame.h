// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SSaveGame.generated.h"

USTRUCT()
struct  FActorSaveData
{
	GENERATED_BODY();

public:
	
	//name
	UPROPERTY()
	FString ActorName;

	//location, rotation, scale
	UPROPERTY()
	FTransform ActorTransform;

	UPROPERTY()
	TArray<uint8> ByteData;
};
/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API USSaveGame : public USaveGame
{
	GENERATED_BODY()
	

public:

	UPROPERTY()
	float Credits;

	UPROPERTY()
	TArray<FActorSaveData> SavedActors;

};
