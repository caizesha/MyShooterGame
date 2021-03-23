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
	
	void DrawMatchTimerAndPosition();

	void DrawWeaponHUD();

	void MakeUV(FCanvasIcon& Icon, FVector2D& UVO, FVector2D& UV1, uint16 U, uint16 V, uint16 UL, uint16 VL);

	//��ʽ������
	FString GetTimeString(float TimeSeconds);
	
	//ʵ��׼��
	UPROPERTY()
	FCanvasIcon Crosshair[5];

	UPROPERTY()
	FCanvasIcon HealthBarBg;

	UPROPERTY()
	FCanvasIcon HealthBar;

	UPROPERTY()
	FCanvasIcon HealthIcon;

	FCanvasIcon TimerPlaceBg;

	FCanvasIcon TimerIcon;

	float ScaleUI;

	float offset;

	UPROPERTY()
	UFont* BigFont;

	//������Ӱ
	FFontRenderInfo ShadowFont;

	FColor HUDDark;

	UPROPERTY()
	FCanvasIcon PrimaryWeaponBg;

	UPROPERTY()
	FCanvasIcon PrimaryWeaponIcon;

	//����ͼ��
	UPROPERTY()
	FCanvasIcon PrimaryClipIcon;
};
