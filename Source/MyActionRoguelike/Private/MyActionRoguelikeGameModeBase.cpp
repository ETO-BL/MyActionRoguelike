// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyActionRoguelikeGameModeBase.h"
#include "SCharacter.h"

AMyActionRoguelikeGameModeBase::AMyActionRoguelikeGameModeBase()
{
	DefaultPawnClass = ASCharacter::StaticClass();
}