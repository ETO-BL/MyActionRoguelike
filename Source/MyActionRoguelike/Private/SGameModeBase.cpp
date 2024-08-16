// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameModeBase.h"
#include "AI/SAICharacter.h"
#include "EngineUtils.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameStateBase.h"
#include "SCharacter.h"
#include "SPlayerState.h"
#include "SSaveGame.h"
#include "SGameplayInterface.h"
#include "SMonsterDataAsset.h"
#include "SActionComponent.h"
#include <Kismet/GameplayStatics.h>
#include <SAttributeComponent.h>
#include "Engine/AssetManager.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("BL.SpawnBots"), true, TEXT("Enable Spawning via timer"), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnInterval = 2.0f;
	RespawnDelay = 2.f;
	KillCredits = 10.f;

	MaxPowerUpCount = 5;
	PowerUpInterval = 80.f;

	SlotName = "SaveGame02";

	PlayerStateClass = ASPlayerState::StaticClass();
}

//Before Spawn Actor
void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	if (SelectedSaveSlot.Len() > 0)
	{
		SlotName = SelectedSaveSlot;
	}

	LoadSaveGame();
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBot, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnInterval, true);

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, PowerUpSpawnQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if (PowerUpClass.Num() > 0)
	{
		if (ensure(QueryInstance))
		{
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnPowerUpSpawnQueryCompleted);
		}
	}
}

//配置读取的PlayerState
void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	//Super::前面比创建UI早'BeginPlayingState'
	ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();
	if (PS)
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}


	Super::HandleStartingNewPlayer_Implementation(NewPlayer);//it will call 'BeginPlayingState' in playercontroller
}

void ASGameModeBase::OnPowerUpSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		return;
	}

	//获取所有生成位置
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	//已使用的位置
	TArray<FVector> UsedLocations;

	int32 SpawnCount = 0;

	while (SpawnCount < MaxPowerUpCount && Locations.Num() > 0)
	{
		//获取随机生成位置
		int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);
		FVector PickedLocation = Locations[RandomLocationIndex];
		Locations.RemoveAt(RandomLocationIndex);
		
		bool bValidLocation = true;
		for (FVector Otherlocation : UsedLocations)
		{
			float Distance = (PickedLocation - Otherlocation).Size();

			if (Distance < PowerUpInterval)
			{
				//太近了
				bValidLocation = false;
				break;
			}
		}

		//位置太近了就再找一次,直到找到一个间隔不近的位置
		if (!bValidLocation)
		{
			continue;
		}

		
		//选择一个随机的PowerUpClass
		int32 RandomClassIndex = FMath::RandRange(0, PowerUpClass.Num() - 1);
		TSubclassOf<AActor> RandomPowerUpClass = PowerUpClass[RandomClassIndex];

		FVector Offset(0.0f, 0.0f, 20.0f);  // Z轴偏移量为100单位

		FVector NewLocation = PickedLocation + Offset; 

		GetWorld()->SpawnActor<AActor>(RandomPowerUpClass, NewLocation, FRotator::ZeroRotator);

		//用于距离检测
		UsedLocations.Add(PickedLocation);
		SpawnCount++;
	}
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot Spawning disabled Via 'CVarSpawnBots' "));
		return;
	}
	//判断是否达到生成上线
	int32 NumOfAliveBot = 0;
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;

		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttribute(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			NumOfAliveBot++;
		}
	}

	//设置难度
	float MaxNumOfAliveBot = 10.0f;
	if (DiffcultyCurve)
	{
		MaxNumOfAliveBot = DiffcultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	//如果到达上限就停止生成
	if (NumOfAliveBot >= MaxNumOfAliveBot)
	{
		return;
	}

	//查询生成位置
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnBotQueryCompleted);
	}
}

void ASGameModeBase::OnBotQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot failed."));
		return;
	}

	//生成Minion
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if (Locations.Num() > 0)
	{
		if (MonsterTable)
		{
			//读取数据表, 通过datatable可以配置生成怪物的信息-- Minionclass, Actions, UI......
			TArray<FMonsterInfoRow*> Rows;
			MonsterTable->GetAllRows("", Rows);

			int32 RandomIndex = FMath::RandRange(0, Rows.Num() - 1);
			FMonsterInfoRow* SelectedRow = Rows[RandomIndex];

			//数据表内使用AssetManager管理Asset--软引用, 不立即加载
			UAssetManager* Manager = UAssetManager::GetIfValid();
			if (Manager)
			{
				TArray<FName> Bundle;
				
				//FStreamableDelegate不能用于蓝图,但可以传递动态数量的参数
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonterLoaded, SelectedRow->MonsterId, Locations[0]);
				//需要时手动加载到内存
				Manager->LoadPrimaryAsset(SelectedRow->MonsterId, Bundle, Delegate);
			}
		}
	}
}

//加载后--创建Monster实例
void ASGameModeBase::OnMonterLoaded(FPrimaryAssetId MonsterId, FVector SpawnLocation)
{
	UAssetManager* Manager = UAssetManager::GetIfValid();
	if (Manager)
	{
		//创建Asset实例
		USMonsterDataAsset* MonsterData = Cast<USMonsterDataAsset>(Manager->GetPrimaryAssetObject(MonsterId));
		if (MonsterData)
		{
			//生成
			AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MinionClass, SpawnLocation, FRotator::ZeroRotator);

			//读取Asset配置, 根据Asset添加对应的能力,而不必直接再Minionclass内配置
			if (NewBot)
			{
				USActionComponent* Actioncomp = Cast<USActionComponent>(NewBot->GetComponentByClass(USActionComponent::StaticClass()));
				if (Actioncomp)
				{
					for (TSubclassOf<USAction> Action : MonsterData->Actions)
					{
						Actioncomp->AddAction(Action, NewBot);
					}
				}
			}
		}
	}
}

void ASGameModeBase::KillAll()
{
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;

		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttribute(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this); // Fixme: Pass to Player--depend on credits 
		}
	}
}



void ASGameModeBase::KillActor(AActor* Victim, AActor* Killer)
{
	ASCharacter* Player = Cast<ASCharacter>(Victim);

	if (Player)
	{
		FTimerHandle TimerHandle_RespawnDelay;

		FTimerDelegate Delegate;

		Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());

		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}

	APawn* KillerPawn = Cast<APawn>(Killer);
	if (KillerPawn)
	{
		ASPlayerState* PlayerState = KillerPawn->GetPlayerState<ASPlayerState>();
		if (PlayerState)
		{
			PlayerState->AddCredits(KillCredits);
		}
	}
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		Controller->UnPossess();

		RestartPlayer(Controller);
	}
}


void ASGameModeBase::WriteSaveGame()
{
	//Save Credits
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i ++)
	{
		ASPlayerState* PlayerState = Cast<ASPlayerState>(GameState->PlayerArray[i]);
		if (PlayerState)
		{
			PlayerState->SavePlayerState(CurrentSaveGame);
			break;// single player only for now
		}	
	}

	//clear actors stored before
	CurrentSaveGame->SavedActors.Empty();

	//Save Actors
	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor->Implements<USGameplayInterface>())
		{
			continue;
		}
		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
		ActorData.ActorTransform = Actor->GetTransform();

		//序列化
#pragma region Serialize
		//指定写操作对象  执行时会将数据序列化(Serialize)存入ByteData
		FMemoryWriter MemWriter(ActorData.ByteData);

		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		//只管理UPROPERTY(SaveName)
		Ar.ArIsSaveGame = true;
		//SaveGame UPROPERTIES turned into Binary array
		Actor->Serialize(Ar);
#pragma endregion

		CurrentSaveGame->SavedActors.Add(ActorData);
	}


	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
}

void ASGameModeBase::LoadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		//Credits
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Faild to load data"));
			return;
		}

		//Actors
		for (FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			//只关心GameplayActor
			if (!Actor->Implements<USGameplayInterface>())
			{
				continue;
			}

			//遍历找到对应的Actor设置位置  是不是用TMap更好一点?
			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if (ActorData.ActorName == Actor->GetName())
				{
					Actor->SetActorTransform(ActorData.ActorTransform);

					//读取序列化数据
					FMemoryReader MemoryReader(ActorData.ByteData);

					FObjectAndNameAsStringProxyArchive Ar(MemoryReader, true);
					Ar.ArIsSaveGame = true;
					//反序列化
					Actor->Serialize(Ar);

					ISGameplayInterface::Execute_OnActorLoaded(Actor);

					break;
				}
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Data loaded"));
	}
	else
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));

		UE_LOG(LogTemp, Warning, TEXT("Create new savegame"));
	}

	

}
