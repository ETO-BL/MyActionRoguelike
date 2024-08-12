// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"
#include "PowerUp_Actor.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class MYACTIONROGUELIKE_API APowerUp_Actor : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerUp_Actor();

protected:

	UPROPERTY(ReplicatedUsing = OnRep_IsAlive)
	bool bIsAlive;

	//服务器更新的时候客户端调用
	UFUNCTION()
	void OnRep_IsAlive();

	float RespawnTime;

	FTimerHandle TimeHandler_RespawnTime;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	virtual void Interact_Implementation(APawn* InstigatorPawn);

	void ShowPowerUp();

	void SetPowerUpState(bool bNewState);

	void HideAndCoolDown();

};
