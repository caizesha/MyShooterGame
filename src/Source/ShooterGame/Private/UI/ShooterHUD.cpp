// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterHUD.h"
#include "ShooterPlayerController.h"
#include "ShooterCharacter.h"

AShooterHUD::AShooterHUD()
{
	//进行准星图标标定
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDMainTextureOb(TEXT("/Game/UI/HUD/HUDMain"));
	Crosshair[EShooterCrossHairDirection::Left] = UCanvas::MakeIcon(HUDMainTextureOb.Object, 43, 402, 25, 9);
	Crosshair[EShooterCrossHairDirection::Right] = UCanvas::MakeIcon(HUDMainTextureOb.Object, 88, 402, 25, 9);
	Crosshair[EShooterCrossHairDirection::Top] = UCanvas::MakeIcon(HUDMainTextureOb.Object, 74, 371, 9, 25);
	Crosshair[EShooterCrossHairDirection::Bottom] = UCanvas::MakeIcon(HUDMainTextureOb.Object, 74, 415, 9, 25);
	Crosshair[EShooterCrossHairDirection::Center] = UCanvas::MakeIcon(HUDMainTextureOb.Object, 73, 403, 7, 7);

	//进行血条图标标定
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDAsset02Ob(TEXT("/Game/UI/HUD/HUDAssets02"));
	HealthBarBg = UCanvas::MakeIcon(HUDAsset02Ob.Object, 67, 162, 372, 50);
	HealthBar = UCanvas::MakeIcon(HUDAsset02Ob.Object, 67, 212, 372, 50);
	HealthIcon = UCanvas::MakeIcon(HUDAsset02Ob.Object, 76, 262, 28, 28);

	offset = 20.0f;
}

void AShooterHUD::DrawHUD()
{
	ScaleUI = Canvas->ClipY / 1080.0f;
	DrawCrosshair();
	DrawHealth();
}

#define M 5

void AShooterHUD::DrawCrosshair() 
{
	AShooterPlayerController* Owner = Cast<AShooterPlayerController>(PlayerOwner);
	if (Owner)
	{
		float CenterX = Canvas->ClipX / 2;
		float CenterY = Canvas->ClipY / 2;
		Canvas->DrawIcon(Crosshair[EShooterCrossHairDirection::Center], CenterX - Crosshair[EShooterCrossHairDirection::Center].UL * ScaleUI / 2,
			CenterY - Crosshair[EShooterCrossHairDirection::Center].VL * ScaleUI / 2, ScaleUI);

		Canvas->DrawIcon(Crosshair[EShooterCrossHairDirection::Left], CenterX - Crosshair[EShooterCrossHairDirection::Left].UL * ScaleUI / 2 - M *
			ScaleUI - Crosshair[EShooterCrossHairDirection::Left].UL * ScaleUI, CenterY - Crosshair[EShooterCrossHairDirection::Left].VL * ScaleUI / 2, ScaleUI);

		Canvas->DrawIcon(Crosshair[EShooterCrossHairDirection::Right], CenterX + Crosshair[EShooterCrossHairDirection::Center].UL * ScaleUI / 2 + M *
			ScaleUI, CenterY - Crosshair[EShooterCrossHairDirection::Right].VL * ScaleUI / 2, ScaleUI);

		Canvas->DrawIcon(Crosshair[EShooterCrossHairDirection::Top], CenterX - Crosshair[EShooterCrossHairDirection::Top].UL * ScaleUI / 2,
			CenterY - Crosshair[EShooterCrossHairDirection::Center].VL * ScaleUI / 2 - M * ScaleUI - Crosshair[EShooterCrossHairDirection::Top].VL * ScaleUI, ScaleUI);

		Canvas->DrawIcon(Crosshair[EShooterCrossHairDirection::Bottom], CenterX - Crosshair[EShooterCrossHairDirection::Bottom].UL * ScaleUI / 2,
			CenterY + Crosshair[EShooterCrossHairDirection::Center].VL * ScaleUI / 2 + M * ScaleUI, ScaleUI);
	}
	
}


void AShooterHUD::DrawHealth()
{
	//混合颜色设置
	Canvas->SetDrawColor(FColor::White);

	//绘制血条背景
	const float HealthPosX = (Canvas->ClipX - HealthBarBg.UL *  ScaleUI) / 2;
	const float HealthPosY = Canvas->ClipY - HealthBarBg.VL *  ScaleUI;
	Canvas->DrawIcon(HealthBarBg, HealthPosX, HealthPosY, ScaleUI);

	AShooterCharacter* MyPawn = Cast<AShooterCharacter>(GetOwningPawn());
	const float HealthAmount = FMath::Min(1.0f,(float)MyPawn->GetCurrentHealth() / MyPawn->GetMaxHealth());
	
	FCanvasTileItem TileItem(FVector2D(HealthPosX, HealthPosY), HealthBar.Texture->Resource,
		FVector2D(HealthBar.UL * HealthAmount * ScaleUI, HealthBar.VL * ScaleUI), FLinearColor::White);

	//求取新的进度图标的位置
	MakeUV(HealthBar, TileItem.UV0, TileItem.UV1, HealthBar.U, HealthBar.V, HealthBar.UL*HealthAmount, HealthBar.VL);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);

	Canvas->DrawIcon(HealthIcon, HealthPosX + offset * ScaleUI, HealthPosY + (HealthBarBg.VL - HealthIcon.VL)*ScaleUI/2 , ScaleUI);
}

void AShooterHUD::MakeUV(FCanvasIcon& Icon, FVector2D& UVO, FVector2D& UV1, uint16 U, uint16 V, uint16 UL, uint16 VL)
{
	if (Icon.Texture)
	{
		float Width = Icon.Texture->GetSurfaceWidth();
		float Height = Icon.Texture->GetSurfaceHeight();
		
		//归一化
		UVO = FVector2D(U / Width, V / Height);
		UV1 = FVector2D((U + UL) / Width, (V + VL) / Height);
	}
}