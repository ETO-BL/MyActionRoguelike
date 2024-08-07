// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SWorldUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API USWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()
	

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
