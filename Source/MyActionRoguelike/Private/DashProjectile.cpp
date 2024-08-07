// Fill out your copyright notice in the Description page of Project Settings.


#include "DashProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"


ADashProjectile::ADashProjectile()
{
	TeleportDelay = 0.2f;
	DetonateDelay = 0.2f;

	MoveComp->InitialSpeed = 6000.f;
}

void ADashProjectile::BeginPlay()
{
	Super::BeginPlay();

	//设置爆炸延迟
	GetWorldTimerManager().SetTimer(TimerHandle_DelayDetonate, this, &ADashProjectile::Explode, DetonateDelay);
}


void ADashProjectile::Explode_Implementation()
{
	//已经命中,删除爆炸动画延迟
	GetWorldTimerManager().ClearTimer(TimerHandle_DelayDetonate);
	
	//播放例子特效和声音
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());

	//设置延迟,等待角色传送
	FTimerHandle TimerHandle_TeleportDelay;
	GetWorldTimerManager().SetTimer(TimerHandle_TeleportDelay, this, &ADashProjectile::TeleportInstigator, TeleportDelay);
}

void ADashProjectile::TeleportInstigator()
{
	AActor* TeleportActor = GetInstigator();
	if (ensure(TeleportActor))
	{
		TeleportActor->TeleportTo(GetActorLocation(), TeleportActor->GetActorRotation(), false, false);

		//应用镜头晃动效果
		APawn* TeleportPawn = Cast<APawn>(TeleportActor);
		APlayerController* PC = nullptr;
		if (ensure(TeleportPawn))
		{
			PC = Cast<APlayerController>(TeleportPawn->GetController());
		}

		if (ensure(PC) && PC->IsLocalController())
		{
			PC->ClientPlayCameraShake(DashShake);
			UE_LOG(LogTemp, Warning, TEXT("Shake!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
		}

		//传送后再销毁
		Destroy();
	}
}


