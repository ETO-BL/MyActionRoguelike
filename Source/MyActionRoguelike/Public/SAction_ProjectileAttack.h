// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SAction_ProjectileAttack.generated.h"

/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API USAction_ProjectileAttack : public USAction
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, Category = "SocketName")
	FName SocketName;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;	

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AnimDelay;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TObjectPtr<UParticleSystem>  StartAttackEffect;

	UFUNCTION()
	void AttackDelay_Elapsed(ACharacter* InstigatorCharacter);

public:

	virtual void StartAction_Implementation(AActor* Intigator) override;

	USAction_ProjectileAttack();

};
