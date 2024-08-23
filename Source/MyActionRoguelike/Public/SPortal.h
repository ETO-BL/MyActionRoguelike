// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "SPortal.generated.h"

class UBoxComponent;
class UCameraComponent;
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
	TObjectPtr<UStaticMeshComponent> PortalPlane;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> TeleportDetection;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> PlayerNearByBounds;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneCaptureComponent2D> PortalSceneCapture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UMaterial* PortalMaterial;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UArrowComponent* ForwardDirection;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* PlayerCamera;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	UPROPERTY(EditDefaultsOnly)
	float PortalQuality;

	FVector LastLocation;

	bool bLastInFront;

protected:

	virtual void PostInitializeComponents()override;

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	void UpdateSceneCapture();

	void CheckResolution();

	void SetClipPlanes();

	void ShouleTeleport();

	bool IsCrossingPortal(FVector ActorLocation, FVector PortalLocation, FVector PortalNormal);

	void TeleportPlayer();

	FVector UpdateLocation(FVector OldLocation);

	FRotator UpdateRotation(FRotator OldRotation);
};
