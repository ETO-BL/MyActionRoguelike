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

	//���ñ�ը�ӳ�
	GetWorldTimerManager().SetTimer(TimerHandle_DelayDetonate, this, &ADashProjectile::Explode, DetonateDelay);
}


void ADashProjectile::Explode_Implementation()
{
	//�Ѿ�����,ɾ����ը�����ӳ�
	GetWorldTimerManager().ClearTimer(TimerHandle_DelayDetonate);
	
	//����������Ч������
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());

	//�����ӳ�,�ȴ���ɫ����
	FTimerHandle TimerHandle_TeleportDelay;
	GetWorldTimerManager().SetTimer(TimerHandle_TeleportDelay, this, &ADashProjectile::TeleportInstigator, TeleportDelay);
}

void ADashProjectile::TeleportInstigator()
{
	AActor* TeleportActor = GetInstigator();
	if (ensure(TeleportActor))
	{
		TeleportActor->TeleportTo(GetActorLocation(), TeleportActor->GetActorRotation(), false, false);

		//Ӧ�þ�ͷ�ζ�Ч��
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

		//���ͺ�������
		Destroy();
	}
}


