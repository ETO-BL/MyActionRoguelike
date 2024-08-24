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
#include "SCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

ASPortal::ASPortal()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;
	PortalScenePlane = CreateDefaultSubobject<UStaticMeshComponent>("ProtalMeshComp");
	PortalScenePlane->SetupAttachment(RootComponent);
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
	PlayerCamera->SetActive(true);

	LinkedPortal = nullptr;
	PortalQuality = 1.f;
	PrimaryActorTick.bCanEverTick = true;
	bIsSynchronized = true;
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
		PortalScenePlane->SetMaterial(0, PortalMAT);

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

	//��ȡ��ҿ�����
	PC = GetWorld()->GetFirstPlayerController();
}

void ASPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSceneCapture();
	CheckResolution();
	ShouldTeleport(); 
	UpdateLinkedCamera();
}

void ASPortal::UpdateSceneCapture()
{
	if (!LinkedPortal)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Linked Portal!"));
		return;
	}

	//��ȡ������λ�ú���ת
	APlayerCameraManager* CameraManager = PC->PlayerCameraManager;
	if (CameraManager)
	{
		//���λ�ú������ת
		FVector CameraLocation = CameraManager->GetTransformComponent()->GetComponentTransform().GetLocation();
		FRotator CameraRotation = CameraManager->GetTransformComponent()->GetComponentTransform().GetRotation().Rotator();
	
		FVector NewLocation = UpdateLocation(CameraLocation, LinkedPortal);
		FRotator NewRotation = UpdateRotation(CameraRotation, LinkedPortal);
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
	FVector PlaneLocation = PortalScenePlane->GetComponentLocation();
	FVector ForwardVector = ForwardDirection->GetForwardVector();

	FVector ClipDirection = PlaneLocation + (ForwardVector * (-3.f));

	PortalSceneCapture->ClipPlaneBase = ClipDirection;
	PortalSceneCapture->ClipPlaneNormal = ForwardVector;
}

void ASPortal::ShouldTeleport()
{
	TArray<AActor*> OverlappingActors;
	PlayerNearByBounds->GetOverlappingActors(OverlappingActors);
	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (!IsValid(OverlappingActor))
		{
			return;
		}
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
	
	APawn* PlayerPawn = PC->GetPawn();

	//�����ж��Ƿ񴩹�������
	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FVector PortalLocation = GetActorLocation();
	FVector PortalNormal = ForwardDirection->GetForwardVector();
	//��ɫ����portal
	if (IsCrossingPortal(PlayerLocation, PortalLocation, PortalNormal))
	{

		TeleportPlayer();

		//�����������
		if (bIsSynchronized) //��ͬһ��--->��ɫ�������portal����--�л�Ϊ�����ŵ����
		{
			bIsSynchronized = false;
			LinkedPortal->bIsSynchronized = false;
			PC->SetViewTargetWithBlend(this, VTBlend_Linear);
			UE_LOG(LogTemp, Warning, TEXT("Camera Set"));
		}
		else //���ൽһ��--�л��ؽ�ɫ���
		{
			bIsSynchronized = true;
			LinkedPortal->bIsSynchronized = true;
			PC->SetViewTargetWithBlend(PC->GetCharacter(), VTBlend_Linear);
			UE_LOG(LogTemp, Warning, TEXT("Camera Set back"));
		}
	}
	//else  //û��portal���������ȥ��
	//{
	//	//�����������
	//	if (bIsSynchronized) //��ͬһ�ൽ��ɫ�������portal����--�л�Ϊ�����ŵ����
	//	{
	//		FHitResult HitPortal;
	//		FVector Start = PC->PlayerCameraManager->GetCameraLocation();
	//		FVector End = PC->GetCharacter()->GetActorLocation();
	//		FCollisionQueryParams QueryParams;
	//		QueryParams.AddIgnoredActors(OverlappingActors);
	//		FCollisionResponseParams Params;
	//		
	//		if (GetWorld()->LineTraceSingleByChannel(HitPortal, Start, End, ECC_WorldStatic, QueryParams, Params))
	//		{
	//			//�������ȥ��
	//			if (HitPortal.Component == PortalScenePlane)
	//			{
	//				bIsSynchronized = false;
	//				LinkedPortal->bIsSynchronized = false;
	//				//���в�ͬ,û�������ŵ����������Ӧ���Ǵ���linkedportal
	//				PC->SetViewTargetWithBlend(LinkedPortal, VTBlend_Linear);
	//			}
	//		}
	//	}
	//	else //���ൽһ��--�л��ؽ�ɫ���
	//	{
	//		FHitResult HitPortal;
	//		FVector Start = PC->PlayerCameraManager->GetCameraLocation();
	//		FVector End = Start + PC->PlayerCameraManager->GetTransformComponent()->GetForwardVector() * 1000.f;
	//		
	//		FCollisionQueryParams QueryParams;
	//		QueryParams.AddIgnoredActors(OverlappingActors);
	//		FCollisionResponseParams Params;
	//		if (GetWorld()->LineTraceSingleByChannel(HitPortal, Start, End, ECC_WorldStatic, QueryParams, Params))
	//		{
	//			//�������ȥ��
	//			if (HitPortal.Component != LinkedPortal->PortalScenePlane)
	//			{
	//				bIsSynchronized = true;
	//				LinkedPortal->bIsSynchronized = true;
	//				//���в�ͬ,û�������ŵ����������Ӧ���Ǵ���linkedportal
	//				PC->SetViewTargetWithBlend(PC->GetCharacter(), VTBlend_Linear);
	//			}
	//		}
	//	}
	//}
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

	// ��ȡƽ������ĳߴ磨��������һ����̬����ƽ�棩
	FVector PlaneSize = PortalScenePlane->GetStaticMesh()->GetBoundingBox().GetSize() * PortalScenePlane->GetComponentScale(); 

	// �� IntersectionPoint ͶӰ��ƽ�汾�ؿռ�
	FVector LocalIntersectionPoint = PortalScenePlane->GetComponentTransform().InverseTransformPosition(IntersectionPoint); 

	// ���ͶӰ���Ƿ���ƽ������ı߽���
	bool bIsWithinBounds = FMath::Abs(LocalIntersectionPoint.X) <= PlaneSize.X * 0.5f &&
		FMath::Abs(LocalIntersectionPoint.Y) <= PlaneSize.Y * 0.5f;

	if (bIsIntersecting && !bIsInFront && bLastInFront && bIsWithinBounds)
	{
		bIsCrossing = true;
	}

	bLastInFront = bIsInFront;
	LastLocation = PlayerLocation;

	return bIsCrossing;
}

void ASPortal::TeleportPlayer()
{
	APawn* PlayerPawn = PC->GetPawn();
	if (PlayerPawn)
	{
		//��ɫ���λ�ú������ת
		FVector PlayerLocation = PlayerPawn->GetActorLocation();
		FRotator PlayerRotation = PlayerPawn->GetActorRotation();

		FVector NewLocation = UpdateLocation(PlayerLocation, LinkedPortal);
		FRotator NewRotation = UpdateRotation(PlayerRotation, LinkedPortal);

		PlayerPawn->SetActorLocationAndRotation(NewLocation, NewRotation);

		FRotator PCRotation = PC->GetControlRotation();
		FRotator PCNewRotation = UpdateRotation(PCRotation, LinkedPortal);
		PC->SetControlRotation(PCNewRotation);
	}
	 
	ACharacter* MyCharacter = Cast<ACharacter>(PlayerPawn);
	if (MyCharacter) 
	{
		FVector OldVelocity = MyCharacter->GetCharacterMovement()->Velocity; 
		MyCharacter->GetCharacterMovement()->Velocity = UpdateVelocity(OldVelocity, LinkedPortal); 
	}
}

FVector ASPortal::UpdateLocation(FVector OldLocation, ASPortal* OtherPortal)
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
	FVector NewLocation = OtherPortal->GetActorTransform().TransformPosition(RelativeLocation);
	return NewLocation;
}

FRotator ASPortal::UpdateRotation(FRotator OldRotation, ASPortal* OtherPortal)
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
	FRotator NewRotation = OtherPortal->GetActorTransform().TransformRotation(MirroredRotation.Quaternion()).Rotator();

	return NewRotation;
}

FVector ASPortal::UpdateVelocity(FVector OldVelocity, ASPortal* OtherPortal)
{
	//��ȡ�ٶȷ������
	FVector VelocityNormal = OldVelocity.GetSafeNormal();

	FTransform PortalTransform = GetActorTransform();
	//����ٶȷ���
	FVector RelativeVelocity = PortalTransform.InverseTransformVector(VelocityNormal);
	
	//��λ����
	FVector MirrorNormalX(1, 0, 0);
	FVector MirrorNormalY(0, 1, 0);

	RelativeVelocity = RelativeVelocity.MirrorByVector(MirrorNormalX);
	RelativeVelocity = RelativeVelocity.MirrorByVector(MirrorNormalY);

	FVector NewVelocity = OtherPortal->GetActorTransform().TransformVector(RelativeVelocity);

	float Length = OldVelocity.Length();

	return NewVelocity * Length;
}

void ASPortal::UpdateLinkedCamera()
{
	APlayerCameraManager* CameraManager = PC->PlayerCameraManager;

	//��ȡ���λ�ú���ת
	FVector CameraLocation = CameraManager->GetTransformComponent()->GetComponentTransform().GetLocation();
	FRotator CameraRotation = CameraManager->GetTransformComponent()->GetComponentTransform().GetRotation().Rotator(); 

	//���ö�Ӧ��PlayerCamera��λ�ú���ת
	FVector NewLocation = LinkedPortal->UpdateLocation(CameraLocation, this);  
	FRotator NewRotation = LinkedPortal->UpdateRotation(CameraRotation, this);

	PlayerCamera->SetWorldLocationAndRotation(NewLocation, NewRotation);
}

