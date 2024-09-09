// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include "SGameplayInterface.h"
#include "DrawDebugHelpers.h"
#include "SWorldUserWidget.h"
#include "Engine/EngineTypes.h"

static TAutoConsoleVariable<bool> CVarDrawDebugInteraction(TEXT("BL.DrawDebugInteraction"), false, TEXT("Enable Debug line and Sphere for Interaction"), ECVF_Cheat);


USInteractionComponent::USInteractionComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.2f;

	TraceDistance = 500.f;
	TraceRadius = 30.f;

	CollisionChannel = ECC_WorldDynamic;
}

void USInteractionComponent::PrimaryInteract()
{
	ServerInteract(FocusedActor);
}

void USInteractionComponent::ServerInteract_Implementation(AActor* InFocusedActor)
{
	if (InFocusedActor == nullptr)
	{
		return;
	}
	APawn* MyPawn = Cast<APawn>(GetOwner());
	ISGameplayInterface::Execute_Interact(InFocusedActor, MyPawn);
}


void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//检测逻辑只在客户端执行
	APawn* MyPawn = Cast<APawn>(GetOwner());
	if (MyPawn->IsLocallyControlled())
	{
		FindBestInteractable();
	}
}

//检测交互物体
void USInteractionComponent::FindBestInteractable()
{
	//命令行控制DebugMessage
	bool bDrawDebug = CVarDrawDebugInteraction.GetValueOnGameThread();


#pragma region TraceFromEye


	FVector End;
	FVector EyeLocation;
	FRotator EyeRotation;

	AActor* MyOwner = GetOwner();
	//获取EyeLocation, EyeRotation
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	End = EyeLocation + (EyeRotation.Vector() * TraceDistance);
#pragma endregion

#pragma region Sweep
	//碰撞查询参数
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	//用于存储Sweep的结果
	TArray<FHitResult> Hits;
	//设置Sweep半径
	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);
	//扫描到的结果
	bool BlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);
	//是否扫描到
	FColor LineColor = BlockingHit ? FColor::Green : FColor::Red;

#pragma endregion

	//默认值为Nullptr | 如果不再聚焦到物体也就再设置为nullptr
	FocusedActor = nullptr;

	//用于绘制球体Sweep的结果
	FHitResult TmpHit;
	//对于每个结果
	for (FHitResult Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			if (HitActor->Implements<USGameplayInterface>())
			{
				FocusedActor = HitActor;
				TmpHit = Hit;
				break;
			}
		}
	}

	//如果聚焦到物体, 且尚未绘制交互UI
	if (FocusedActor)
	{
		if (DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass))//好习惯
		{
			DefaultWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}
		//设置交互UI,借由Tick绘制
		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->AttachedActor = FocusedActor;

			if (!DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	//不再聚焦到交互物体
	else
	{
		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->RemoveFromParent();
		}
	}
	//Debug
	if (bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), TmpHit.ImpactPoint, TraceRadius, 16, LineColor, false, 2.0f);
		DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 0.0f);
	}
}