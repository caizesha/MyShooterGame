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

	void DrawHealth();
	
	void MakeUV(FCanvasIcon& Icon, FVector2D& UVO, FVector2D& UV1, uint16 U, uint16 V, uint16 UL, uint16 VL);
	//ÊµÏÖ×¼ÐÇ
	UPROPERTY()
	FCanvasIcon Crosshair[5];

	UPROPERTY()
	FCanvasIcon HealthBarBg;

	UPROPERTY()
	FCanvasIcon HealthBar;

	UPROPERTY()
	FCanvasIcon HealthIcon;

	float ScaleUI;

	float offset;

};
