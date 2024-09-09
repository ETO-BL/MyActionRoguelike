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

	//����߼�ֻ�ڿͻ���ִ��
	APawn* MyPawn = Cast<APawn>(GetOwner());
	if (MyPawn->IsLocallyControlled())
	{
		FindBestInteractable();
	}
}

//��⽻������
void USInteractionComponent::FindBestInteractable()
{
	//�����п���DebugMessage
	bool bDrawDebug = CVarDrawDebugInteraction.GetValueOnGameThread();


#pragma region TraceFromEye


	FVector End;
	FVector EyeLocation;
	FRotator EyeRotation;

	AActor* MyOwner = GetOwner();
	//��ȡEyeLocation, EyeRotation
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	End = EyeLocation + (EyeRotation.Vector() * TraceDistance);
#pragma endregion

#pragma region Sweep
	//��ײ��ѯ����
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	//���ڴ洢Sweep�Ľ��
	TArray<FHitResult> Hits;
	//����Sweep�뾶
	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);
	//ɨ�赽�Ľ��
	bool BlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);
	//�Ƿ�ɨ�赽
	FColor LineColor = BlockingHit ? FColor::Green : FColor::Red;

#pragma endregion

	//Ĭ��ֵΪNullptr | ������پ۽�������Ҳ��������Ϊnullptr
	FocusedActor = nullptr;

	//���ڻ�������Sweep�Ľ��
	FHitResult TmpHit;
	//����ÿ�����
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

	//����۽�������, ����δ���ƽ���UI
	if (FocusedActor)
	{
		if (DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass))//��ϰ��
		{
			DefaultWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}
		//���ý���UI,����Tick����
		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->AttachedActor = FocusedActor;

			if (!DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	//���پ۽�����������
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