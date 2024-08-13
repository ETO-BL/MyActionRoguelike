// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"
#include "SItemChest.generated.h"

class UStaticMeshComponent;

UCLASS()
class MYACTIONROGUELIKE_API ASItemChest : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float TargetPitch;

	void Interact_Implementation(APawn* InstigatorPawn);

	void OnActorLoaded_Implementation();
	
public:	
	// Sets default values for this actor's properties
	ASItemChest();

protected:

	//服务器更新bLidOpen到客户端时,客户端自动调用OnRep_LidOpend
	UPROPERTY(ReplicatedUsing = OnRep_LidOpened, BlueprintReadOnly, SaveGame)
	bool bLidOpend;

	UFUNCTION()
	void OnRep_LidOpened();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> LidMesh;

};
