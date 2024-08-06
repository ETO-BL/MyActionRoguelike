// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USInteractionComponent;
class UAnimMontage;
class USAttributeComponent;
class UParticleSystemComponent;

UCLASS()
class MYACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

protected:
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> DashProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> BlackholeProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	FTimerHandle TimerHandle_PrimaryAttack;

	FTimerHandle TimerHandle_Dash;

	FTimerHandle TimerHandle_BlackholeAttack;

	UPROPERTY(EditAnywhere, Category = "Attack | PrimaryAttack")
	float AnimDelay = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Target")
	float SweepRadius;

	UPROPERTY(EditAnywhere, Category = "Target")
	float SweepDistanceFallback;

	UPROPERTY(EditAnywhere, Category = "SocketName")
	FName SocketName;

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UCameraComponent> CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USInteractionComponent> InteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USAttributeComponent> AttributeComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackEffect")
	TObjectPtr<UParticleSystem>  StartAttackEffect;


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float value);
	
	void MoveRight(float value);

	void PrimaryAttack();

	void PrimaryAttack_TimeElapsed();

	void PrimaryInteract();

	void Dash();

	void Dash_TimeElasped();

	void BlackholeAttack();

	void BlackholeAttack_TimeElapsed();

	void SpawnProjectile(TSubclassOf<AActor> ClassToSpawn);

	void ApplyStartAttackEffect();

	UFUNCTION()
	void OnHealthChange(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	virtual void PostInitializeComponents()override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
