// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"

class UPawnSensingComponent;
class USAttributeComponent;
class USWorldUserWidget;
class USActionComponent;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USActionComponent>  ActionComp;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TargetActorKey;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParamName;

	FTimerHandle FadeOutTimerHandle;

	float FadeOutStartTime;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float FadeOutDuration;

	UFUNCTION()
	void FadeOutUpdate();

	void UpdateMaterialFadeOut(float Alpha);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	USWorldUserWidget* ActiveHealthBar;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USWorldUserWidget> SpottedWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	USWorldUserWidget* SopttedWidget;

	UFUNCTION()
	void OnHealthChange(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	UFUNCTION()
	void SetTargetActor(AActor* NewTarget);

	UFUNCTION()
	AActor* GetTargetActor() const;

	UFUNCTION()
	void OnSeePawn(APawn* Pawn);


	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPawnSeen();
};
