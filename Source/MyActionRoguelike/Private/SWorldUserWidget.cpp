// Fill out your copyright notice in the Description page of Project Settings.


#include "SWorldUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"

void USWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	//死亡后移除
	if (!IsValid(AttachedActor))
	{
		RemoveFromParent();

		return;
	}

	FVector2D ScreenPosition;

	bool bIsOnScreen = UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, ScreenPosition);



	if (bIsOnScreen)
	{
		float Scale =  UWidgetLayoutLibrary::GetViewportScale(this);
		//窗口化,适应窗口大小
		ScreenPosition /= Scale;			

		if(ParentSizeBox)
		{
			ParentSizeBox->SetRenderTranslation(ScreenPosition);
		}
	}

	if (ParentSizeBox)
	{
		ParentSizeBox->SetVisibility(bIsOnScreen ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}
