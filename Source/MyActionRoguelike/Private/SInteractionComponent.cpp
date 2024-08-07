// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include "SGameplayInterface.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void USInteractionComponent::PrimaryInteract()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	AActor* MyOwner = GetOwner();

	FVector End;
	FVector EyeLocation;
	FRotator EyeRotation;
	//»ñÈ¡EyeLocation, EyeRotation
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	End = EyeLocation + (EyeRotation.Vector() * 1000);

	//FHitResult Hit;
	//bool BlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);

	TArray<FHitResult> Hits;

	float Radius = 30.0f;
	FCollisionShape Shape;
	Shape.SetSphere(Radius);

	bool BlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);

	FColor LineColor = BlockingHit ? FColor::Green : FColor::Red;

	FHitResult TmpHit;
	for (FHitResult	Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();

		if (HitActor)
		{
			if (HitActor->Implements<USGameplayInterface>())
			{
				APawn* MyPawn = Cast<APawn>(MyOwner);

				ISGameplayInterface::Execute_Interact(HitActor, MyPawn);
				TmpHit = Hit;
				break;
			}
		}
		
	}
	DrawDebugSphere(GetWorld(), TmpHit.ImpactPoint, Radius, 16, LineColor, false, 2.0f);
	
	DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 0.0f);
}