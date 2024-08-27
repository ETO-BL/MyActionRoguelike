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
	

	// 设置裁剪面
	SetClipPlanes();

	//获取玩家控制器
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
	//获取玩家相机位置和旋转
	APlayerCameraManager* CameraManager = PC->PlayerCameraManager;
	if (CameraManager)
	{
		//相对位置和相对旋转
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
//		//用于判断是否穿过传送门
//		FVector PlayerLocation = PCM->GetActorLocation();
//		FVector PortalLocation = GetActorLocation();
//		FVector PortalNormal = ForwardDirection->GetForwardVector();
//
//		bIsCorssing = IsCrossingPortal(PlayerLocation, PortalLocation, PortalNormal);
//		//FVector CameraLocation = PCM->GetPawnViewLocation();
//		//bIsCameraCorssing = IsCrossingPortal(CameraLocation, PortalLocation, PortalNormal);
//		//UE_LOG(LogTemp, Warning, TEXT("IsCrossing: %s"), bIsCorssing ? TEXT("true") : TEXT("false"));
//		//UE_LOG(LogTemp, Warning, TEXT("bIsCameraCorssing: %s"), bIsCameraCorssing ? TEXT("true") : TEXT("false"));
//		//角色穿过portal
//		if (bIsCorssing)
//		{
//			TeleportPlayer();
//			//传送相机设置
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
//					//略有不同,没进传送门但是相机进了应该是传入linkedportal
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
//					//回来了
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
//				//略有不同,没进传送门但是相机进了应该是传入linkedportal
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
//				//回来了
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

	//是否通过了portal
	float DotProductResult = FVector::DotProduct(PlayerLocation - PortalLocation, PortalNormal);
	if (DotProductResult >= 0)
	{
		bIsInFront = true;
	}

	FPlane PortalDetectionPlane(PortalLocation, PortalNormal);
	
	const FVector IntersectionPoint = FMath::LinePlaneIntersection(LastLocation, PlayerLocation, PortalDetectionPlane);

	const bool bIsIntersecting = (FVector::DotProduct(IntersectionPoint - LastLocation, IntersectionPoint - PlayerLocation) <= 0);

	// 获取平面网格的尺寸（假设它是一个静态网格平面）
	FVector PlaneSize = PortalScenePlane->GetStaticMesh()->GetBoundingBox().GetSize() * PortalScenePlane->GetComponentScale(); 

	// 将 IntersectionPoint 投影到平面本地空间
	FVector LocalIntersectionPoint = PortalScenePlane->GetComponentTransform().InverseTransformPosition(IntersectionPoint); 

	// 检查投影点是否在平面网格的边界内
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
	//角色相对位置和相对旋转
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

	//反转传送门位置
	FVector Scale = PortalTransform.GetScale3D();
	Scale.X *= -1;
	Scale.Y *= -1;
	PortalTransform.SetScale3D(Scale);

	//相对位置
	FVector RelativeLocation = PortalTransform.InverseTransformPosition(OldLocation);
	//对应的世界位置
	FVector NewLocation = OtherPortal->GetActorTransform().TransformPosition(RelativeLocation);
	return NewLocation;
}

FRotator ASPortal::UpdateRotation(FRotator OldRotation, ASPortal* OtherPortal)
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
	FRotator NewRotation = OtherPortal->GetActorTransform().TransformRotation(MirroredRotation.Quaternion()).Rotator();

	return NewRotation;
}

FVector ASPortal::UpdateVelocity(FVector OldVelocity, ASPortal* OtherPortal)
{
	//获取速度方向标量
	FVector VelocityNormal = OldVelocity.GetSafeNormal();

	FTransform PortalTransform = GetActorTransform();
	//相对速度方向
	FVector RelativeVelocity = PortalTransform.InverseTransformVector(VelocityNormal);
	
	//单位向量
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
	//获取相机位置和旋转
	FVector CameraLocation = PCM->GetPawnViewLocation();
	FRotator CameraRotation = PCM->GetViewRotation(); 

	//设置对应的PlayerCamera的位置和旋转
	FVector NewLocation = LinkedPortal->UpdateLocation(CameraLocation, this);  
	FRotator NewRotation = LinkedPortal->UpdateRotation(CameraRotation, this);

	PlayerCamera->SetWorldLocationAndRotation(NewLocation, NewRotation);
}

void ASPortal::SetSceneMat()
{
	// 创建材质和设置材质
	if (PortalMaterial)
	{
		//创建动态材质实例
		UMaterialInstanceDynamic* PortalMAT = UMaterialInstanceDynamic::Create(PortalMaterial, this);
		PortalScenePlane->SetMaterial(0, PortalMAT);

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

}

void ASPortal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPortal, bIsCrossing);
	DOREPLIFETIME(ASPortal, bIsInFront);
	DOREPLIFETIME(ASPortal, bIsSynchronized);
	DOREPLIFETIME(ASPortal, bLastInFront);
}