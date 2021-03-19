// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

/**
 * 
 */

enum EShooterCrossHairDirection
{
	Left,
	Right,
	Center,
	Top,
	Bottom
};

UCLASS()
class SHOOTERGAME_API AShooterHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AShooterHUD();

	virtual void DrawHUD() override;

	void DrawCrosshair();
	//ÊµÏÖ×¼ÐÇ
	UPROPERTY()
	FCanvasIcon Crosshair[5];

	float ScaleUI;
	
};
