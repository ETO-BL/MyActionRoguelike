// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction_Effect.h"
#include "SAction_Effect_Thorns.generated.h"

/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API USAction_Effect_Thorns : public USAction_Effect
{
	GENERATED_BODY()

public:

	USAction_Effect_Thorns();

	void StartAction_Implementation(AActor* Instigator)override;

	void StopAction_Implementation(AActor* Instigator)override;


protected:

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float ReflectFraction;
	
	UFUNCTION()
	void OnHealthChange(AActor* InstigatorActor, USAttributeComponent* AttributeComp, float NewHealth, float Delta);

};
