// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
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
	TObjectPtr<UStaticMeshComponent> PortalPlane;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> BoxComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneCaptureComponent2D> PortalSceneCapture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UMaterial* PortalMaterial;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UArrowComponent* ForwardDirection;


	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	UPROPERTY(EditDefaultsOnly)
	float PortalQuality;

	virtual void PostInitializeComponents()override;

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void UpdateSceneCapture();

	UFUNCTION(BlueprintCallable)
	void CheckResolution();

	void SetClipPlanes();
};
