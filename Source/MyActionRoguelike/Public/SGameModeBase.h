// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "SGameModeBase.generated.h"

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
class USSaveGame;
/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:

	FString SlotName;

	UPROPERTY()
	USSaveGame* CurrentSaveGame;

	FTimerHandle TimerHandle_SpawnBot;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> MinionClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnInterval;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UEnvQuery> SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DiffcultyCurve;

	UPROPERTY(EditAnywhere, Category = "CharacterRespawn")
	float RespawnDelay;

	

	UPROPERTY(EditDefaultsOnly, Category = "Credits")
	float KillCredits; 

	UPROPERTY(EditDefaultsOnly, Category = "Credits")
	int32 MaxPowerUpCount;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUpClass")
	TObjectPtr<UEnvQuery> PowerUpSpawnQuery;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUpClass")
	TArray<TSubclassOf<AActor>> PowerUpClass;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUpClass")
	float PowerUpInterval;

	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	void OnBotQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

public:

	ASGameModeBase();

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void StartPlay() override;
	
	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)override;

	UFUNCTION(Exec)
	void KillAll();

	virtual void KillActor(AActor* Victim, AActor* Killer);

	UFUNCTION()
	void RespawnPlayerElapsed(AController* PlayerToRespawn);

	UFUNCTION()
	void OnPowerUpSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void WriteSaveGame();

	void LoadSaveGame();
	
};
