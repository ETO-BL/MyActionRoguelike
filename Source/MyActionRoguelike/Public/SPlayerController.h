// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnChanged, APawn*, NewPawn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, APlayerState*, NewPlayerState);

UCLASS()
class MYACTIONROGUELIKE_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY()
	UUserWidget* PauseMenu;

	UFUNCTION(BlueprintCallable)
	void TooglePauseMenu();

	void SetupInputComponent()override;

	UPROPERTY(BlueprintAssignable)
	FOnPawnChanged OnPawnChanged;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChanged OnPlayerStateReceived;

	virtual void SetPawn(APawn* NewPawn) override; 

#pragma region UI
	//����ҿ�������PlayerController��׼���ÿ�ʼ��Ϸʱ�����á�ͨ�����ڳ�ʼ��һЩ�� BeginPlay �п��ܹ���ִ�е����� 
	//�ڶ�����Ϸ�Ŀͻ����ϣ�BeginPlayingState ��һ���ʺϳ�ʼ�� UI������桢HUD����ʱ��
	//��Ϊ�ڴ�ʱ����Ϸ�����б�Ҫ���ݣ����� PlayerState�����Ѿ�׼���á����� BeginPlay �У����ܻ����������δ��ȫͬ��������--��playerstate
	virtual void BeginPlayingState() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintBeginPlayingState();
#pragma endregion

	//Ҳ��ͨ���¼�ͬ��
	void OnRep_PlayerState()override;

};
