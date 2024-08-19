// Fill out your copyright notice in the Description page of Project Settings.


#include "SPortal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMathLibrary.h"

ASPortal::ASPortal()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;
	PortalPlane = CreateDefaultSubobject<UStaticMeshComponent>("ProtalMeshComp");
	PortalPlane->SetupAttachment(RootComponent);

	BoxComp = CreateDefaultSubobject<UBoxComponent>("BoxComp");
	BoxComp->SetupAttachment(RootComponent);

	PortalSceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCaptureComp");
	PortalSceneCapture->SetupAttachment(RootComponent);
		
	ForwardDirection = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ForwardDirection->SetupAttachment(RootComponent);
	ForwardDirection->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

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
}

void ASPortal::UpdateSceneCapture()
{
	if (!LinkedPortal)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Linked Portal!"));
		return;
	}

	FTransform PortalTransform = GetActorTransform();

	//��ת������λ��
	FVector Scale = PortalTransform.GetScale3D();
	Scale.X *= -1;
	Scale.Y *= -1;
	PortalTransform.SetScale3D(Scale);

	//��ȡ������λ�ú���ת
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	CameraManager->GetTransformComponent()->GetComponentTransform().GetLocation();
	if (CameraManager)
	{
		//���λ�ú������ת
		FVector CameraLocation = CameraManager->GetTransformComponent()->GetComponentTransform().GetLocation();
		FVector RelativeLocation = PortalTransform.InverseTransformPosition(CameraLocation);

		FRotator CameraRotation = CameraManager->GetTransformComponent()->GetComponentTransform().GetRotation().Rotator();	
		FRotator RelativeRotation = PortalTransform.InverseTransformRotation(CameraRotation.Quaternion()).Rotator();
		
		// �� FRotator ת��Ϊ FMatrix
		FMatrix RotationMatrix = FRotationMatrix(RelativeRotation);

		// ��ȡ��ת�������������
		FVector XAxis = RotationMatrix.GetUnitAxis(EAxis::X);
		FVector YAxis = RotationMatrix.GetUnitAxis(EAxis::Y);
		FVector ZAxis = RotationMatrix.GetUnitAxis(EAxis::Z);
		
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

		//ת��Ϊ����λ�ú�������ת
		FVector SceneCpatureNewLocation = LinkedPortal->GetActorTransform().TransformPosition(RelativeLocation);
		FRotator SceneCaptureNewRotation = LinkedPortal->GetActorTransform().TransformRotation(MirroredRotation.Quaternion()).Rotator();
		LinkedPortal->PortalSceneCapture->SetWorldLocationAndRotation(SceneCpatureNewLocation, SceneCaptureNewRotation);
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







