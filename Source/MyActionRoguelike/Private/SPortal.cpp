// Fill out your copyright notice in the Description page of Project Settings.


#include "SPortal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"

ASPortal::ASPortal()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetupAttachment(RootComponent);

	BoxComp = CreateDefaultSubobject<UBoxComponent>("BoxComp");
	BoxComp->SetupAttachment(RootComponent);

	SceneCaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCaptureComp");
	SceneCaptureComp->SetupAttachment(RootComponent);

	LinkedPortal = nullptr;
}

void ASPortal::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ASPortal::OnOverlapBegin);
}

void ASPortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (OtherActor && OtherActor != this && LinkedPortal)
	{
		FVector NewLocation = LinkedPortal->BoxComp->GetComponentLocation();
		//暂时禁用碰撞
		LinkedPortal->BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		OtherActor->SetActorLocation(NewLocation);

		// 一段时间后重新启用碰撞
		FTimerHandle TimerHandle_Teleport;

		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "BoxEnableCollision", LinkedPortal);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Teleport, Delegate, 3.f, false);
	}
}

void ASPortal::BoxEnableCollision(ASPortal* TargetPortal)
{
	if (TargetPortal && TargetPortal->BoxComp)
	{
		TargetPortal->BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void ASPortal::UpdateCaptureComponent()
{
	if (!LinkedPortal || !SceneCaptureComp)
	{
		return;
	}


	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		APawn* PlayerPawn = PC->GetPawn();
		// 获取玩家在传送门1的局部坐标系中的位置和旋转
		FVector LocalPosition = MeshComp->GetComponentTransform().InverseTransformPosition(PlayerPawn->GetActorLocation());
		FRotator LocalRotation = MeshComp->GetComponentTransform().InverseTransformRotation(PlayerPawn->GetActorRotation().Quaternion()).Rotator();

		// 将局部位置和旋转转换到传送门2的世界坐标系
		FVector NewPosition = LinkedPortal->MeshComp->GetComponentTransform().TransformPosition(LocalPosition);
		FRotator NewRotation = LinkedPortal->MeshComp->GetComponentTransform().TransformRotation(LocalRotation.Quaternion()).Rotator();

		// 设置SceneCaptureComponent2D的位置和旋转
		SceneCaptureComp->SetWorldLocation(NewPosition);
		SceneCaptureComp->SetWorldRotation(NewRotation);
	}
}

void ASPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LinkedPortal && SceneCaptureComp)
	{
		UpdateCaptureComponent();
	}
}




