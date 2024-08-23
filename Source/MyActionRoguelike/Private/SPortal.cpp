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

	// 创建材质和设置材质
	if (PortalMaterial)
	{
		//创建动态材质实例
		UMaterialInstanceDynamic* PortalMAT = UMaterialInstanceDynamic::Create(PortalMaterial, this);
		PortalPlane->SetMaterial(0, PortalMAT);

		//获取视口大小,创建渲染目标
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

		//更新动态材质
		PortalMAT->SetTextureParameterValue(FName("Texture"), RenderTarget);

		if (LinkedPortal)
		{
			LinkedPortal->PortalSceneCapture->TextureTarget = RenderTarget;
		}
	}

	// 设置裁剪面
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

	//获取玩家相机位置和旋转
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	CameraManager->GetTransformComponent()->GetComponentTransform().GetLocation();
	if (CameraManager)
	{
		//相对位置和相对旋转
		FVector CameraLocation = CameraManager->GetTransformComponent()->GetComponentTransform().GetLocation();
		FRotator CameraRotation = CameraManager->GetTransformComponent()->GetComponentTransform().GetRotation().Rotator();
	
		FVector NewLocation = UpdateLocation(CameraLocation);
		FRotator NewRotation = UpdateRotation(CameraRotation);
		LinkedPortal->PortalSceneCapture->SetWorldLocationAndRotation(NewLocation, NewRotation);
	}
}

void ASPortal::CheckResolution()
{
	//缩放到相同大小
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

	//是否通过了portal
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
			//角色相对位置和相对旋转
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

	//反转传送门位置
	FVector Scale = PortalTransform.GetScale3D();
	Scale.X *= -1;
	Scale.Y *= -1;
	PortalTransform.SetScale3D(Scale);

	//相对位置
	FVector RelativeLocation = PortalTransform.InverseTransformPosition(OldLocation);
	//对应的世界位置
	FVector NewLocation = LinkedPortal->GetActorTransform().TransformPosition(RelativeLocation);
	return NewLocation;
}

FRotator ASPortal::UpdateRotation(FRotator OldRotation)
{
	FTransform PortalTransform = GetActorTransform();

	//反转传送门位置
	FVector Scale = PortalTransform.GetScale3D();
	Scale.X *= -1;
	Scale.Y *= -1;
	PortalTransform.SetScale3D(Scale);

	FRotator RelativeRotation = PortalTransform.InverseTransformRotation(OldRotation.Quaternion()).Rotator();
	// 将 FRotator 转换为 FMatrix
	FMatrix RotationMatrix = FRotationMatrix(RelativeRotation);
	// 获取旋转后的三个轴向量
	FVector XAxis = RotationMatrix.GetUnitAxis(EAxis::X);
	FVector YAxis = RotationMatrix.GetUnitAxis(EAxis::Y);
	FVector ZAxis = RotationMatrix.GetUnitAxis(EAxis::Z);
	//单位向量
	FVector MirrorNormalX(1, 0, 0);
	FVector MirrorNormalY(0, 1, 0);
	// 分别镜像两个轴
	FVector MirroredXAxis = UKismetMathLibrary::MirrorVectorByNormal(XAxis, MirrorNormalX);
	MirroredXAxis = UKismetMathLibrary::MirrorVectorByNormal(MirroredXAxis, MirrorNormalY);

	FVector MirroredYAxis = UKismetMathLibrary::MirrorVectorByNormal(YAxis, MirrorNormalX);
	MirroredYAxis = UKismetMathLibrary::MirrorVectorByNormal(MirroredYAxis, MirrorNormalY);
	// 重新组合为旋转矩阵
	FMatrix MirroredMatrix = FMatrix(MirroredXAxis, MirroredYAxis, ZAxis, FVector::ZeroVector);
	FRotator MirroredRotation = MirroredMatrix.Rotator();

	//对应世界旋转
	FRotator NewRotation = LinkedPortal->GetActorTransform().TransformRotation(MirroredRotation.Quaternion()).Rotator();

	return NewRotation;
}

