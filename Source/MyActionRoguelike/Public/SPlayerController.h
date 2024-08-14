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
	//当玩家控制器（PlayerController）准备好开始游戏时被调用。通常用于初始化一些在 BeginPlay 中可能过早执行的内容 
	//在多人游戏的客户端上，BeginPlayingState 是一个适合初始化 UI（如界面、HUD）的时机
	//因为在此时，游戏的所有必要数据（例如 PlayerState）都已经准备好。而在 BeginPlay 中，可能会出现数据尚未完全同步的问题--如playerstate
	virtual void BeginPlayingState() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintBeginPlayingState();
#pragma endregion

	//也可通过事件同步
	void OnRep_PlayerState()override;

};
