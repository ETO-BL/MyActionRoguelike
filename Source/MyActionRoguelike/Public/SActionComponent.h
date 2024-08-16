// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SActionComponent.generated.h"

class USAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionChanged, USActionComponent*, OwningComp, USAction*, ChangedAction);



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYACTIONROGUELIKE_API USActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPROPERTY(BlueprintAssignable)
	FOnActionChanged OnActionStarted;

	UPROPERTY(BlueprintAssignable) 
	FOnActionChanged OnActionStoped;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer ActiveGameplayTags;

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddAction(TSubclassOf<USAction> ActionClass, AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void RemoveAction(USAction* ActionClass);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool StartActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool StopActionByName(AActor* Instigator, FName ActionName);


	USActionComponent();

protected:

	UFUNCTION(Server, Reliable)
	void ServerStartAction(AActor* Instigator, FName AcitonName);

	UFUNCTION(Server, Reliable)
	void ServerStopAction(AActor* Instigator, FName AcitonName);

	//游戏开始时就配置好能力
	UPROPERTY(EditDefaultsOnly, Category = "Actions")
	TArray<TSubclassOf<USAction>> DefaultActions;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<USAction*> Actions;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
};
