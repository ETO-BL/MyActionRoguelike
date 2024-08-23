// Fill out your copyright notice in the Description page of Project Settings.


#include "SPortal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Camera/CameraComponent.h"
#include "UObject/UObjectGlobals.h"
#include "Math/Plane.h"
#include "Math/Vector.h"
#include "Kismet/KismetMathLibrary.h"

ASPortal::ASPortal()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;
	PortalPlane = CreateDefaultSubobject<UStaticMeshComponent>("ProtalMeshComp");
	PortalPlane->SetupAttachment(RootComponent);
	TeleportDetection = CreateDefaultSubobject<UBoxComponent>("BoxComp");
	TeleportDetection->SetupAttachment(RootComponent);

	PlayerNearByBounds = CreateDefaultSubobject<UBoxComponent>("NearByBoxComp");
	PlayerNearByBounds->SetupAttachment(RootComponent);

	PortalSceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCaptureComp");
	PortalSceneCapture->SetupAttachment(RootComponent);
		
	ForwardDirection = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ForwardDirection->SetupAttachment(RootComponent);
	ForwardDirection->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(RootComponent);


	LinkedPortal = nullptr;
	PortalQuality = 1.f;
	PrimaryActorTick.bCanEverTick = true;
}

void ASPortal::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASPortal::BeginPlay()
{
	Super::BeginPlay();
	

	SetTickGroup(TG_PostUpdateWork);

	// �������ʺ����ò���
	if (PortalMaterial)
	{
		//������̬����ʵ��
		UMaterialInstanceDynamic* PortalMAT = UMaterialInstanceDynamic::Create(PortalMaterial, this);
		PortalPlane->SetMaterial(0, PortalMAT);

		//��ȡ�ӿڴ�С,������ȾĿ��
		FVector2D ViewportSize;
		UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
		if (ViewportClient)
		{
			ViewportClient->GetViewportSize(ViewportSize);
		}
		
		int32 Width = FMath::TruncToInt(ViewportSize.X * PortalQuality);
		int32 Height = FMath::TruncToInt(ViewportSize.Y * PortalQuality);
		
		RenderTarget = NewObject<UTextureRenderTarget2D>();
		if (RenderTarget)
		{
			RenderTarget->InitCustomFormat(Width, Height, PF_FloatRGBA, false);
		}

		//���¶�̬����
		PortalMAT->SetTextureParameterValue(FName("Texture"), RenderTarget);

		if (LinkedPortal)
		{
			LinkedPortal->PortalSceneCapture->TextureTarget = RenderTarget;
		}
	}

	// ���òü���
	SetClipPlanes();
}

void ASPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSceneCapture();
	CheckResolution();
	ShouleTeleport();
}

void ASPortal::UpdateSceneCapture()
{
	if (!LinkedPortal)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Linked Portal!"));
		return;
	}

	//��ȡ������λ�ú���ת
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	CameraManager->GetTransformComponent()->GetComponentTransform().GetLocation();
	if (CameraManager)
	{
		//���λ�ú������ת
		FVector CameraLocation = CameraManager->GetTransformComponent()->GetComponentTransform().GetLocation();
		FRotator CameraRotation = CameraManager->GetTransformComponent()->GetComponentTransform().GetRotation().Rotator();
	
		FVector NewLocation = UpdateLocation(CameraLocation);
		FRotator NewRotation = UpdateRotation(CameraRotation);
		LinkedPortal->PortalSceneCapture->SetWorldLocationAndRotation(NewLocation, NewRotation);
	}
}

void ASPortal::CheckResolution()
{
	//���ŵ���ͬ��С
	FVector2D ViewportSize;
	UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
	if (ViewportClient)
	{
		ViewportClient->GetViewportSize(ViewportSize);
	}

	int32 Width = FMath::TruncToInt(ViewportSize.X * PortalQuality);
	int32 Height = FMath::TruncToInt(ViewportSize.Y * PortalQuality);

	int32 RTWidth = RenderTarget->SizeX;
	int32 RTHeight = RenderTarget->SizeY;

	if (Width == RTWidth && Height == RTHeight)
	{
		return;
	}

	RenderTarget->ResizeTarget(Width, Height);
}

void ASPortal::SetClipPlanes()
{
	PortalSceneCapture->bEnableClipPlane = true;
	FVector PlaneLocation = PortalPlane->GetComponentLocation();
	FVector ForwardVector = ForwardDirection->GetForwardVector();

	FVector ClipDirection = PlaneLocation + (ForwardVector * (-3.f));

	PortalSceneCapture->ClipPlaneBase = ClipDirection;
	PortalSceneCapture->ClipPlaneNormal = ForwardVector;
}

void ASPortal::ShouleTeleport()
{
	TArray<AActor*> OverlappingActors;
	PlayerNearByBounds->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (!IsValid(OverlappingActor))
		{
			return;
		}

		TArray<AActor*> TeleportActors;
		TeleportDetection->GetOverlappingActors(TeleportActors);

		for (AActor* TeleportActor : TeleportActors)
		{
			if (!IsValid(TeleportActor))
			{
				return;
			}
		}
	}
	
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		APawn* PlayerPawn = PC->GetPawn();
		if (PlayerPawn)
		{
			FVector PlayerLocation = PlayerPawn->GetActorLocation();
			FVector PortalLocation = GetActorLocation();
			FVector PortalNormal = ForwardDirection->GetForwardVector();
			if (IsCrossingPortal(PlayerLocation, PortalLocation, PortalNormal))
			{
				TeleportPlayer();
				UE_LOG(LogTemp, Warning, TEXT("IS CROSSING"));
			}
		}				
	}
}

bool ASPortal::IsCrossingPortal(FVector PlayerLocation, FVector PortalLocation, FVector PortalNormal)
{
	bool bIsInFront = false;
	bool bIsCrossing = false;

	//�Ƿ�ͨ����portal
	float DotProductResult = FVector::DotProduct(PlayerLocation - PortalLocation, PortalNormal);
	if (DotProductResult >= 0)
	{
		bIsInFront = true;
	}

	FPlane PortalDetectionPlane(PortalLocation, PortalNormal);
	
	const FVector IntersectionPoint = FMath::LinePlaneIntersection(LastLocation, PlayerLocation, PortalDetectionPlane);

	const bool bIsIntersecting = (FVector::DotProduct(IntersectionPoint - LastLocation, IntersectionPoint - PlayerLocation) <= 0);

	if (bIsIntersecting && !bIsInFront && bLastInFront)
	{
		bIsCrossing = true;
	}

	bLastInFront = bIsInFront;
	LastLocation = PlayerLocation;

	return bIsCrossing;
}

void ASPortal::TeleportPlayer()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		APawn* PlayerPawn = PC->GetPawn();
		if (PlayerPawn)
		{
			//��ɫ���λ�ú������ת
			FVector PlayerLocation = PlayerPawn->GetActorLocation();
			FRotator PlayerRotation = PlayerPawn->GetActorRotation();

			FVector NewLocation = UpdateLocation(PlayerLocation);
			FRotator NewRotation = UpdateRotation(PlayerRotation);

			PlayerPawn->SetActorLocationAndRotation(NewLocation, NewRotation);

			FRotator PCRotation = PC->GetControlRotation();
			FRotator PCNewRotation = UpdateRotation(PCRotation);
			PC->SetControlRotation(PCNewRotation);
		}
	}
}

FVector ASPortal::UpdateLocation(FVector OldLocation)
{
	FTransform PortalTransform = GetActorTransform();

	//��ת������λ��
	FVector Scale = PortalTransform.GetScale3D();
	Scale.X *= -1;
	Scale.Y *= -1;
	PortalTransform.SetScale3D(Scale);

	//���λ��
	FVector RelativeLocation = PortalTransform.InverseTransformPosition(OldLocation);
	//��Ӧ������λ��
	FVector NewLocation = LinkedPortal->GetActorTransform().TransformPosition(RelativeLocation);
	return NewLocation;
}

FRotator ASPortal::UpdateRotation(FRotator OldRotation)
{
	FTransform PortalTransform = GetActorTransform();

	//��ת������λ��
	FVector Scale = PortalTransform.GetScale3D();
	Scale.X *= -1;
	Scale.Y *= -1;
	PortalTransform.SetScale3D(Scale);

	FRotator RelativeRotation = PortalTransform.InverseTransformRotation(OldRotation.Quaternion()).Rotator();
	// �� FRotator ת��Ϊ FMatrix
	FMatrix RotationMatrix = FRotationMatrix(RelativeRotation);
	// ��ȡ��ת�������������
	FVector XAxis = RotationMatrix.GetUnitAxis(EAxis::X);
	FVector YAxis = RotationMatrix.GetUnitAxis(EAxis::Y);
	FVector ZAxis = RotationMatrix.GetUnitAxis(EAxis::Z);
	//��λ����
	FVector MirrorNormalX(1, 0, 0);
	FVector MirrorNormalY(0, 1, 0);
	// �ֱ���������
	FVector MirroredXAxis = UKismetMathLibrary::MirrorVectorByNormal(XAxis, MirrorNormalX);
	MirroredXAxis = UKismetMathLibrary::MirrorVectorByNormal(MirroredXAxis, MirrorNormalY);

	FVector MirroredYAxis = UKismetMathLibrary::MirrorVectorByNormal(YAxis, MirrorNormalX);
	MirroredYAxis = UKismetMathLibrary::MirrorVectorByNormal(MirroredYAxis, MirrorNormalY);
	// �������Ϊ��ת����
	FMatrix MirroredMatrix = FMatrix(MirroredXAxis, MirroredYAxis, ZAxis, FVector::ZeroVector);
	FRotator MirroredRotation = MirroredMatrix.Rotator();

	//��Ӧ������ת
	FRotator NewRotation = LinkedPortal->GetActorTransform().TransformRotation(MirroredRotation.Quaternion()).Rotator();

	return NewRotation;
}

