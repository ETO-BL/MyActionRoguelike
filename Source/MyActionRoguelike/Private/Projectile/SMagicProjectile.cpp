// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/SMagicProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "ActionSystem/SAttributeComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "GameplayFunctionLibrary.h"
#include "ActionSystem/SActionComponent.h"
#include "ActionSystem/SAction.h"


// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
}

//命中目标
void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("HitActor!!!!!!!"));

	if (OtherActor && OtherActor != GetInstigator())
	{
		
		USActionComponent* ActionComp = Cast<USActionComponent>(OtherActor->GetComponentByClass(USActionComponent::StaticClass()));
		
		//检查是否可以反弹 
		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			UE_LOG(LogTemp, Warning, TEXT("Parrying"));
			MoveComp->Velocity = -MoveComp->Velocity;
			SetInstigator(Cast<APawn>(OtherActor));
			return;
		}

		
		//造成伤害, Explode()播放效果和声音,然后销毁
		if (UGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
		{
			
			Explode();

			if (ActionComp && BurnningActionClass)
			{
				ActionComp->AddAction(BurnningActionClass, GetInstigator());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AddActionFailed"));
			}
		}
	}
}
