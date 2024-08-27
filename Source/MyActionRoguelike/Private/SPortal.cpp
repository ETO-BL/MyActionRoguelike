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
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

ASPortal::ASPortal()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;
	PortalScenePlane = CreateDefaultSubobject<UStaticMeshComponent>("ProtalMeshComp");
	PortalScenePlane->SetupAttachment(RootComponent);
	PortalTestPlane = CreateDefaultSubobject<UStaticMeshComponent>("TextPlaneComp");
	PortalTestPlane->SetupAttachment(RootComponent);
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
	SetReplicates(true);
}

void ASPortal::BeginPlay()
{
	Super::BeginPlay();

	SetTickGroup(TG_PostUpdateWork);

	FTimerHandle Thimerhandle_PortalSceneSet;

	GetWorldTimerManager().SetTimer(Thimerhandle_PortalSceneSet, this, &ASPortal::SetSceneMat, 0.3f, false);
	

	// ���òü���
	SetClipPlanes();

	//��ȡ��ҿ�����
	PC = GetWorld()->GetFirstPlayerController();
	if (PC == nullptr) return;
	PCM = Cast<ACharacter>(PC->GetCharacter()); 
	if (PCM == nullptr) return;
}

void ASPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSceneCapture();
	CheckResolution();

	//UpdateLinkedCamera();
	//ShouldTeleport(); 
	
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
	if (RenderTarget == nullptr)
	{
		return;
	}

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

//void ASPortal::ShouldTeleport()
//{
//	bool bISValid = false;
//
//	TArray<AActor*> OverlappingActors;
//	PlayerNearByBounds->GetOverlappingActors(OverlappingActors);
//	for (AActor* OverlappingActor : OverlappingActors)
//	{
//		if (!IsValid(OverlappingActor))
//		{
//			return;
//		}
//	}
//	TArray<AActor*> TeleportActors;
//	TeleportDetection->GetOverlappingActors(TeleportActors);
//	for (AActor* OverlappingActor : OverlappingActors)
//	{
//		if (!IsValid(OverlappingActor))
//		{
//			bISValid = false;
//			break;
//		}
//	}
//
//
//	if (bISValid)
//	{
//		
//		//�����ж��Ƿ񴩹�������
//		FVector PlayerLocation = PCM->GetActorLocation();
//		FVector PortalLocation = GetActorLocation();
//		FVector PortalNormal = ForwardDirection->GetForwardVector();
//
//		bIsCorssing = IsCrossingPortal(PlayerLocation, PortalLocation, PortalNormal);
//		//FVector CameraLocation = PCM->GetPawnViewLocation();
//		//bIsCameraCorssing = IsCrossingPortal(CameraLocation, PortalLocation, PortalNormal);
//		//UE_LOG(LogTemp, Warning, TEXT("IsCrossing: %s"), bIsCorssing ? TEXT("true") : TEXT("false"));
//		//UE_LOG(LogTemp, Warning, TEXT("bIsCameraCorssing: %s"), bIsCameraCorssing ? TEXT("true") : TEXT("false"));
//		//��ɫ����portal
//		if (bIsCorssing)
//		{
//			TeleportPlayer();
//			//�����������
//			if (bIsSynchronized)
//			{
//				bIsSynchronized = false;
//				LinkedPortal->bIsSynchronized = bIsSynchronized;
//				PC->SetViewTargetWithBlend(this, VTBlend_Linear);
//			}
//			else
//			{
//				bIsSynchronized = true;
//				LinkedPortal->bIsSynchronized = bIsSynchronized;
//				PC->SetViewTargetWithBlend(PC->GetCharacter(), VTBlend_Linear);
//			}
//		}
//		else
//		{
//#pragma region Preset
//			FString Message = FString::Printf(TEXT("bIsSynchronized: %s"), bIsSynchronized ? TEXT("true") : TEXT("false"));
//			GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, Message);
//
//			FHitResult HitPortal;
//			FVector Start = PC->PlayerCameraManager->GetCameraLocation();
//			FVector End;
//			FCollisionQueryParams QueryParams;
//			QueryParams.AddIgnoredActor(this);
//			FCollisionResponseParams Params;
//#pragma endregion
//			if (bIsSynchronized)
//			{
//				End = PCM->GetActorLocation();
//				GetWorld()->LineTraceSingleByChannel(HitPortal, Start, End, ECC_GameTraceChannel1, QueryParams, Params);
//				if (HitPortal.Component == PortalTestPlane)
//				{
//					UE_LOG(LogTemp, Warning, TEXT("HITCOMPONENT"));
//					bIsSynchronized = false;
//					LinkedPortal->bIsSynchronized = false;
//					//���в�ͬ,û�������ŵ����������Ӧ���Ǵ���linkedportal
//					PC->SetViewTargetWithBlend(LinkedPortal, VTBlend_Linear);
//				}
//			}
//			else
//			{
//				End = Start + PC->PlayerCameraManager->GetTransformComponent()->GetForwardVector() * 1000.f;
//				GetWorld()->LineTraceSingleByChannel(HitPortal, Start, End, ECC_GameTraceChannel1, QueryParams, Params);
//				if (HitPortal.Component != LinkedPortal->PortalTestPlane)
//				{
//					UE_LOG(LogTemp, Warning, TEXT("Not HITCOMPONENT!"));
//					bIsSynchronized = true;
//					LinkedPortal->bIsSynchronized = true;
//					//������
//					PC->SetViewTargetWithBlend(PC->GetCharacter(), VTBlend_Linear);
//				}
//			}
//		}
//	}
//	else
//	{
//#pragma region Preset
//		FString Message = FString::Printf(TEXT("bIsSynchronized: %s"), bIsSynchronized ? TEXT("true") : TEXT("false"));
//		GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, Message);
//
//		FHitResult HitPortal;
//		FVector Start = PC->PlayerCameraManager->GetCameraLocation();
//		FVector End;
//		FCollisionQueryParams QueryParams;
//		QueryParams.AddIgnoredActor(this);
//		FCollisionResponseParams Params;
//#pragma endregion
//		if (bIsSynchronized)
//		{
//			End = PCM->GetActorLocation();
//			GetWorld()->LineTraceSingleByChannel(HitPortal, Start, End, ECC_GameTraceChannel1, QueryParams, Params);
//			if (HitPortal.Component == PortalTestPlane)
//			{
//				UE_LOG(LogTemp, Warning, TEXT("HITCOMPONENT"));
//				bIsSynchronized = false;
//				LinkedPortal->bIsSynchronized = false;
//				//���в�ͬ,û�������ŵ����������Ӧ���Ǵ���linkedportal
//				PC->SetViewTargetWithBlend(LinkedPortal, VTBlend_Linear);
//			}
//		}
//		else
//		{
//			End = Start + PC->PlayerCameraManager->GetTransformComponent()->GetForwardVector() * 1000.f;
//			GetWorld()->LineTraceSingleByChannel(HitPortal, Start, End, ECC_GameTraceChannel1, QueryParams, Params);
//			if (HitPortal.Component != LinkedPortal->PortalTestPlane)
//			{
//				UE_LOG(LogTemp, Warning, TEXT("Not HITCOMPONENT!"));
//				bIsSynchronized = true;
//				LinkedPortal->bIsSynchronized = true;
//				//������
//				PC->SetViewTargetWithBlend(PC->GetCharacter(), VTBlend_Linear);
//			}
//		}
//	}
//	
//}

void ASPortal::IsCrossingPortal(FVector PlayerLocation, FVector PortalLocation, FVector PortalNormal)
{
	bIsInFront = false;
	bIsCrossing = false;

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
	bool bIsWithinBounds = FMath::Abs(LocalIntersectionPoint.X) <= PlaneSize.X * 0.3f &&
		FMath::Abs(LocalIntersectionPoint.Y) <= PlaneSize.Y * 0.3f;

	if (bIsIntersecting && !bIsInFront && bLastInFront && bIsWithinBounds)
	{
		bIsCrossing = true;
	}

	bLastInFront = bIsInFront;
	LastLocation = PlayerLocation;
}

void ASPortal::TeleportPlayer_Implementation()
{
	if (!PCM)
	{
		PC = GetWorld()->GetFirstPlayerController();
		if (PC == nullptr) return;
		PCM = Cast<ACharacter>(PC->GetCharacter());
		if (PCM == nullptr) return;
	}
	//��ɫ���λ�ú������ת
	FVector PlayerLocation = PCM->GetActorLocation();
	FRotator PlayerRotation = PCM->GetActorRotation();

	FVector NewLocation = UpdateLocation(PlayerLocation, LinkedPortal);
	FRotator NewRotation = UpdateRotation(PlayerRotation, LinkedPortal);

	PCM->SetActorLocationAndRotation(NewLocation, NewRotation);

	FRotator PCRotation = PC->GetControlRotation();
	FRotator PCNewRotation = UpdateRotation(PCRotation, LinkedPortal);
	PC->SetControlRotation(PCNewRotation);

	FVector OldVelocity = PCM->GetCharacterMovement()->Velocity;
	PCM->GetCharacterMovement()->Velocity = UpdateVelocity(OldVelocity, LinkedPortal);

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
	if (!PCM)
	{
		PC = GetWorld()->GetFirstPlayerController();
		if (PC == nullptr) return;
		PCM = Cast<ACharacter>(PC->GetCharacter());
		if (PCM == nullptr) return;
	}
	//��ȡ���λ�ú���ת
	FVector CameraLocation = PCM->GetPawnViewLocation();
	FRotator CameraRotation = PCM->GetViewRotation(); 

	//���ö�Ӧ��PlayerCamera��λ�ú���ת
	FVector NewLocation = LinkedPortal->UpdateLocation(CameraLocation, this);  
	FRotator NewRotation = LinkedPortal->UpdateRotation(CameraRotation, this);

	PlayerCamera->SetWorldLocationAndRotation(NewLocation, NewRotation);
}

void ASPortal::SetSceneMat()
{
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

}

void ASPortal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPortal, bIsCrossing);
	DOREPLIFETIME(ASPortal, bIsInFront);
	DOREPLIFETIME(ASPortal, bIsSynchronized);
	DOREPLIFETIME(ASPortal, bLastInFront);
}