// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, AActor*, InStigatorActor, USAttributeComponent*, OwningComp, float, NewValue, float, Delta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USAttributeComponent();


public:	

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	static USAttributeComponent* GetAttribute(AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category = "Attribute", meta = (DisplayName = "IsAlive"))
	static bool IsActorAlive(AActor* Actor);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Attribute")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Attribute")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Attribute")
	float Rage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Attribute")
	float MaxRage;

	//NetMulticast用于瞬时的短暂的事件, 只在Server执行  reliable吃性能, unreliable用于优化
	UFUNCTION(NetMulticast, Reliable) //@FixMe: mark as unreliable once we move the "State" out of character
	void MulticastHealthChanged(AActor* Instigator, float Newhealth, float Delta);

	UFUNCTION(NetMulticast, Unreliable) //@FixMe: mark as unreliable once we move the "State" out of character
	void MulticastRageChanged(AActor* Instigator, float NewRage, float Delta);

public:	

	UPROPERTY(BlueprintAssignable, Category = "Attribute")
	FOnAttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Attribute")
	FOnAttributeChanged OnRageChanged;


	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable)
	bool ApplyHealthChanged(AActor* Instigator, float Delta);

	UFUNCTION(BlueprintCallable)
	USAttributeComponent* GetAttributeComponent(AActor* FromActor);

	UFUNCTION(BlueprintCallable)
	bool IsFullHealth();

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth();

	UFUNCTION(BlueprintCallable)
	float GetHealth();

	UFUNCTION(BlueprintCallable)
	bool Kill(AActor* Instigator);

	UFUNCTION(BlueprintCallable)
	bool AddRage(AActor* Instigator, float Delta);

	UFUNCTION(BlueprintCallable)
	float GetRage();

	UFUNCTION(BlueprintCallable)
	float GetMaxRage();

	UFUNCTION(BlueprintCallable)
	bool IsFullRage();

};
