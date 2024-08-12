// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, ASPlayerState*, PlayerState, float, NewCredits, float, Delta);
/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRe_Credits, Category = "Credits")
	float Credits;

	UFUNCTION()
	void OnRe_Credits(float OldCredits);

	
public:
	
	UPROPERTY(BlueprintAssignable)
	FOnCreditsChanged OnCreditsChanged;
	
	UFUNCTION(BlueprintCallable, Category = "Credits")
	float GetCredits();

	UFUNCTION(BlueprintCallable, Category = "Credits")
	void AddCredits(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Credits")
	bool DelCredits(float Delta);

	ASPlayerState();
};
