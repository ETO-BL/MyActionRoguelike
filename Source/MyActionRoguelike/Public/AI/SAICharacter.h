// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"

class UPawnSensingComponent;
class USAttributeComponent;
UCLASS()
class MYACTIONROGUELIKE_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASAICharacter();

protected:

	virtual void PostInitializeComponents()override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UPawnSensingComponent> PawnSensingComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USAttributeComponent> AttributeComp;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TargetActorKey;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParamName;

	UFUNCTION()
	void OnHealthChange(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	UFUNCTION()
	void SetTargetActor(AActor* NewTarget);

	UFUNCTION()
	AActor* GetTargetActor() const;

	UFUNCTION()
	void OnSeePawn(APawn* Pawn);

};
