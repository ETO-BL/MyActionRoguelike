// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPortal.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USceneCaptureComponent2D;

UCLASS()
class MYACTIONROGUELIKE_API ASPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPortal();

	UPROPERTY(EditAnywhere, Category = "LinkedPortal")
	TObjectPtr<ASPortal> LinkedPortal;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> BoxComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComp;

	virtual void PostInitializeComponents()override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void BoxEnableCollision(ASPortal* TargetPortal);

	void UpdateCaptureComponent();

	virtual void Tick(float DeltaTime) override;
};
