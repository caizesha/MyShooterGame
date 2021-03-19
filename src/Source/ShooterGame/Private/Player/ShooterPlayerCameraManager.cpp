// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterPlayerCameraManager.h"
#include "ShooterCharacter.h"

void AShooterPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);
	AShooterCharacter* MyPawn = Cast<AShooterCharacter>(PCOwner ? PCOwner->GetPawn() : nullptr);
	if (MyPawn)
	{
		MyPawn->OnCameraUpdate(GetCameraLocation(),GetCameraRotation());
	}

}

