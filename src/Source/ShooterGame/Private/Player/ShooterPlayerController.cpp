// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterPlayerController.h"
#include "ShooterPlayerCameraManager.h"

AShooterPlayerController::AShooterPlayerController()
{
	PlayerCameraManagerClass = AShooterPlayerCameraManager::StaticClass();//����Camera(StaticClass�������������䣩
}


