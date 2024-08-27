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
class UASCharacter;

UCLASS()
class MYACTIONROGUELIKE_API ASPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPortal();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LinkedPortal")
	TObjectPtr<ASPortal> LinkedPortal;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PortalScenePlane;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PortalTestPlane;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UBoxComponent> TeleportDetection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UBoxComponent> PlayerNearByBounds;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USceneCaptureComponent2D> PortalSceneCapture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UMaterial* PortalMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UArrowComponent* ForwardDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* PlayerCamera;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	UPROPERTY(EditDefaultsOnly)
	float PortalQuality;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	FVector LastLocation;

	UPROPERTY(VisibleDefaultsOnly, Replicated, BlueprintReadWrite)
	bool bLastInFront;

	UPROPERTY(VisibleDefaultsOnly, Replicated, BlueprintReadWrite)
	bool bIsSynchronized;

	UPROPERTY(VisibleDefaultsOnly, Replicated, BlueprintReadWrite)
	bool bIsCrossing;

	UPROPERTY(VisibleDefaultsOnly, Replicated, BlueprintReadWrite)
	bool bIsInFront;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	APlayerController* PC;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<ACharacter> PCM;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<ACharacter> MirrorCharacter;

	UPROPERTY()
	FTimerHandle MessageTimerHandle;

	FCriticalSection SyncCriticalSection;

protected:
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	void UpdateSceneCapture();

	void CheckResolution();

	UFUNCTION()
	void SetClipPlanes();

	//UFUNCTION(BlueprintCallable)
	//void ShouldTeleport();

	UFUNCTION(BlueprintCallable)
	void IsCrossingPortal(FVector ActorLocation, FVector PortalLocation, FVector PortalNormal);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void TeleportPlayer();

	UFUNCTION(BlueprintCallable)
	FVector UpdateLocation(FVector OldLocation, ASPortal* OtherPortal);

	UFUNCTION(BlueprintCallable)
	FRotator UpdateRotation(FRotator OldRotation, ASPortal* OtherPortal);

	UFUNCTION(BlueprintCallable)
	FVector UpdateVelocity(FVector OldVelocity, ASPortal* OtherPortal);

	UFUNCTION(BlueprintCallable)
	void UpdateLinkedCamera();

	UFUNCTION()
	void SetSceneMat();
};
