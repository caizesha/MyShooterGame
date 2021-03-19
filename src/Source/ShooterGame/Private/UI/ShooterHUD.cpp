// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterHUD.h"
#include "ShooterPlayerController.h"

AShooterHUD::AShooterHUD()
{
	//进行准星图标标定
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDMainTextureOb(TEXT("/Game/UI/HUD/HUDMain"));
	Crosshair[EShooterCrossHairDirection::Left] = UCanvas::MakeIcon(HUDMainTextureOb.Object, 43, 402, 25, 9);
	Crosshair[EShooterCrossHairDirection::Right] = UCanvas::MakeIcon(HUDMainTextureOb.Object, 88, 402, 25, 9);
	Crosshair[EShooterCrossHairDirection::Top] = UCanvas::MakeIcon(HUDMainTextureOb.Object, 74, 371, 9, 25);
	Crosshair[EShooterCrossHairDirection::Bottom] = UCanvas::MakeIcon(HUDMainTextureOb.Object, 74, 415, 9, 25);
	Crosshair[EShooterCrossHairDirection::Center] = UCanvas::MakeIcon(HUDMainTextureOb.Object, 73, 403, 7, 7);


}

void AShooterHUD::DrawHUD()
{
	ScaleUI = Canvas->ClipY / 1080.0f;
	DrawCrosshair();
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


