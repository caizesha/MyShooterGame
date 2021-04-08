// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterHUD.h"
#include "ShooterPlayerController.h"
#include "ShooterCharacter.h"
#include "ShooterGameState.h"
#include "ShooterWeapon.h"

AShooterHUD::AShooterHUD()
{
	//����׼��ͼ��궨
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDMainTextureOb(TEXT("/Game/UI/HUD/HUDMain"));
	Crosshair[EShooterCrossHairDirection::Left] = UCanvas::MakeIcon(HUDMainTextureOb.Object, 43, 402, 25, 9);
	Crosshair[EShooterCrossHairDirection::Right] = UCanvas::MakeIcon(HUDMainTextureOb.Object, 88, 402, 25, 9);
	Crosshair[EShooterCrossHairDirection::Top] = UCanvas::MakeIcon(HUDMainTextureOb.Object, 74, 371, 9, 25);
	Crosshair[EShooterCrossHairDirection::Bottom] = UCanvas::MakeIcon(HUDMainTextureOb.Object, 74, 415, 9, 25);
	Crosshair[EShooterCrossHairDirection::Center] = UCanvas::MakeIcon(HUDMainTextureOb.Object, 73, 403, 7, 7);

	//����Ѫ��ͼ��궨
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDAsset02Ob(TEXT("/Game/UI/HUD/HUDAssets02"));
	HealthBarBg = UCanvas::MakeIcon(HUDAsset02Ob.Object, 67, 162, 372, 50);
	HealthBar = UCanvas::MakeIcon(HUDAsset02Ob.Object, 67, 212, 372, 50);
	HealthIcon = UCanvas::MakeIcon(HUDAsset02Ob.Object, 76, 262, 28, 28);

	TimerPlaceBg = UCanvas::MakeIcon(HUDMainTextureOb.Object, 262, 16, 255, 62);
	TimerIcon = UCanvas::MakeIcon(HUDMainTextureOb.Object, 381, 93, 24, 24);

	offset = 20.0f;
	static ConstructorHelpers::FObjectFinder<UFont> BigFontOb(TEXT("/Game/UI/HUD/Roboto51"));
	BigFont = BigFontOb.Object;

	ShadowFont.bEnableShadow = true;
	HUDDark = FColor(110, 124, 131, 255);

	PrimaryWeaponBg = UCanvas::MakeIcon(HUDMainTextureOb.Object,543, 17, 441, 81);
	PrimaryWeaponIcon = UCanvas::MakeIcon(HUDMainTextureOb.Object, 282, 389, 147, 67);
	PrimaryClipIcon = UCanvas::MakeIcon(HUDMainTextureOb.Object, 148, 151, 62, 51);
}

void AShooterHUD::DrawHUD()
{
	ScaleUI = Canvas->ClipY / 1080.0f;
	DrawCrosshair();
	DrawHealth();
	DrawMatchTimerAndPosition();
	DrawWeaponHUD();
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

		Canvas->DrawIcon(Crosshair[EShooterCrossHairDirection::Left], CenterX - Crosshair[EShooterCrossHairDirection::Center].UL * ScaleUI / 2 - M *
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
	//�����ɫ����
	Canvas->SetDrawColor(FColor::White);

	//����Ѫ������
	const float HealthPosX = (Canvas->ClipX - HealthBarBg.UL *  ScaleUI) / 2;
	const float HealthPosY = Canvas->ClipY - HealthBarBg.VL *  ScaleUI;
	Canvas->DrawIcon(HealthBarBg, HealthPosX, HealthPosY, ScaleUI);

	AShooterCharacter* MyPawn = Cast<AShooterCharacter>(GetOwningPawn());
	if (MyPawn == nullptr)
	{
		return;
	}
	const float HealthAmount = FMath::Min(1.0f,(float)MyPawn->GetCurrentHealth() / MyPawn->GetMaxHealth());
	
	FCanvasTileItem TileItem(FVector2D(HealthPosX, HealthPosY), HealthBar.Texture->Resource,
		FVector2D(HealthBar.UL * HealthAmount * ScaleUI, HealthBar.VL * ScaleUI), FLinearColor::White);

	//��ȡ�µĽ���ͼ���λ��
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
		
		//��һ��
		UVO = FVector2D(U / Width, V / Height);
		UV1 = FVector2D((U + UL) / Width, (V + VL) / Height);
	}
}

void AShooterHUD::DrawMatchTimerAndPosition()
{
	AShooterGameState* const GameState = GetWorld()->GetGameState<AShooterGameState>();
	if (GameState && GameState->RemainingTime > 0)
	{
		//���Ƽ�ʱ������
		const float TimerPosX = Canvas->ClipX - (TimerPlaceBg.UL + offset) * ScaleUI;
		const float TimerPosY = Canvas->OrgY + offset * ScaleUI;

		Canvas->DrawIcon(TimerPlaceBg, TimerPosX, TimerPosY, ScaleUI);

		//���Ƽ�ʱ��
		Canvas->DrawIcon(TimerIcon, TimerPosX + offset*ScaleUI, TimerPosY + (TimerPlaceBg.VL - TimerIcon.VL) / 2 * ScaleUI, ScaleUI);

		//���ʱ������
		FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), BigFont, HUDDark);
		TextItem.EnableShadow(FLinearColor::Black);

		float SizeX, SizeY;
		float TextScale = 0.57f;
		TextItem.FontRenderInfo = ShadowFont;
		TextItem.Scale = FVector2D(TextScale*ScaleUI, TextScale*ScaleUI);
		FString Text = GetTimeString(GameState->RemainingTime);
		//��ȡ���ֻ�������ֵ
		Canvas->StrLen(BigFont, Text, SizeX, SizeY);

		TextItem.Text = FText::FromString(Text);
		TextItem.Position = FVector2D(TimerPosX + offset * 1.5 * ScaleUI + TimerIcon.UL * ScaleUI, TimerPosY + (TimerPlaceBg.VL - SizeY * TextScale) / 2 * ScaleUI);
		Canvas->DrawItem(TextItem);
	}
	
}

FString AShooterHUD::GetTimeString(float TimeSeconds)
{
	//��ȫ���㣺��Ϸʱ�䲻����һ��Сʱ�Ҵ����㣻
	const int32 TotalSeconds = FMath::Max(0, FMath::TruncToInt(TimeSeconds) % 3600);
	const int32 NumMinute = TotalSeconds / 60;
	const int32 NumSecond = TotalSeconds % 60;

	//��ʽ��ʱ���ַ���
	const FString TimeString = FString::Printf(TEXT("%02d:%02d"), NumMinute, NumSecond);
	return TimeString;
}

void AShooterHUD::DrawWeaponHUD()
{
	const float PriWeaponOffsetY = 65;
	const float PriWeaponBoxWidth = 150;
	const float PriClipIconOffset = 25;
	const float AmmoIconCount = 4.0f ;
	const float BoxWidth = 65.0f;

	const float PrimaryWeaponBgPositionY = Canvas->ClipY - Canvas->OrgY - (offset + PriWeaponOffsetY + PrimaryWeaponBg.VL) * ScaleUI;
	
	const float PrimaryWeaponPosX = Canvas->ClipX - Canvas->OrgX - (2 * offset  + (PrimaryWeaponIcon.UL+ PriWeaponBoxWidth)/2) * ScaleUI;
	const float PrimaryWeaponPosY = Canvas->ClipY - Canvas->OrgY - (PriWeaponOffsetY + offset + (PrimaryWeaponBg.VL + PrimaryWeaponIcon.VL) / 2)*ScaleUI;

	const float ClipWidth = PrimaryClipIcon.UL + PriClipIconOffset * (AmmoIconCount - 1);
	const float PriClipPosX = PrimaryWeaponPosX - (BoxWidth + ClipWidth) * ScaleUI;
	const float PriClipPosY = Canvas->ClipY - Canvas->OrgY - (PriWeaponOffsetY + offset + (PrimaryWeaponIcon.VL + PrimaryClipIcon.VL) / 2)*ScaleUI;

	AShooterCharacter* MyPawn = Cast<AShooterCharacter>(GetOwningPawn());
	
	if (MyPawn == nullptr)
	{
		return;
	}

	AShooterWeapon* Weapon = MyPawn->GetCurrentWeapon();

	if (MyPawn&&Weapon)
	{
		//���Ʊ������ǲ���
		const float LeftConrnerWidth = 60.0f;
		FCanvasTileItem TileItem(FVector2D(PriClipPosX - offset * ScaleUI, PrimaryWeaponBgPositionY), PrimaryWeaponBg.Texture->Resource,
			FVector2D(LeftConrnerWidth*ScaleUI, PrimaryWeaponBg.VL*ScaleUI), FLinearColor::White);
		MakeUV(PrimaryWeaponBg, TileItem.UV0, TileItem.UV1, PrimaryWeaponBg.U, PrimaryWeaponBg.V, LeftConrnerWidth, PrimaryWeaponBg.VL);
		TileItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(TileItem);

		//���Ʊ������β���
		const float RestBgWidth = Canvas->ClipX - PriClipPosX - (LeftConrnerWidth + offset + 2 * offset)*ScaleUI;
		TileItem.Position = FVector2D(PriClipPosX + (LeftConrnerWidth - offset)*ScaleUI, PrimaryWeaponBgPositionY);
		TileItem.Size = FVector2D(RestBgWidth, PrimaryWeaponBg.VL * ScaleUI);
		MakeUV(PrimaryWeaponBg, TileItem.UV0, TileItem.UV1, PrimaryWeaponBg.U + (PrimaryWeaponBg.UL - RestBgWidth / ScaleUI), PrimaryWeaponBg.V, RestBgWidth / ScaleUI, PrimaryWeaponBg.VL);
		Canvas->DrawItem(TileItem);

		//��������ͼ�겿��
		Canvas->DrawIcon(PrimaryWeaponIcon, PrimaryWeaponPosX, PrimaryWeaponPosY, ScaleUI);

		//���������ϰ벿��
		float SizeX, SizeY;
		FString Text = FString::FromInt(Weapon->GetCurrentAmmoInClip());
		Canvas->StrLen(BigFont, Text, SizeX, SizeY);

		const float TextOffset = 12;
		const float TopTextScale = 0.73;
		const float TopTextPosX = Canvas->ClipX - Canvas->OrgX - (2 * offset + PriWeaponBoxWidth + (BoxWidth + SizeX * TopTextScale) / 2.0f) * ScaleUI;
		const float TopTextPosY = Canvas->ClipY - Canvas->OrgY - (offset + PriWeaponOffsetY + PrimaryWeaponBg.VL - TextOffset / 2.0f) * ScaleUI;

		FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), BigFont, HUDDark);
		TextItem.EnableShadow (FLinearColor::Black);
		TextItem.Text = FText::FromString(Text);
		TextItem.Scale = FVector2D(TopTextScale*ScaleUI, TopTextScale*ScaleUI);
		TextItem.FontRenderInfo = ShadowFont;
		Canvas->DrawItem(TextItem, TopTextPosX, TopTextPosY);

		//���������°벿��
		float TopTextHeight = SizeY * TopTextScale;
		Text = FString::FromInt(Weapon->GetCurrentAmmo() - Weapon->GetCurrentAmmoInClip());
		Canvas->StrLen(BigFont, Text, SizeX, SizeY);

		const float BottomTextScale = 0.49f;
		const float BottomTextPosX = Canvas->ClipX - Canvas->OrgX - (2 * offset + PriWeaponBoxWidth + (BoxWidth + SizeX * BottomTextScale) / 2.0f) * ScaleUI;
		const float BottomTextPosY = TopTextPosY + (TopTextHeight - 0.8f*TextOffset) * ScaleUI;

		TextItem.Text = FText::FromString(Text);
		TextItem.Scale = FVector2D(BottomTextScale*ScaleUI, BottomTextScale*ScaleUI);
		TextItem.FontRenderInfo = ShadowFont;
		Canvas->DrawItem(TextItem, BottomTextPosX, BottomTextPosY);

		//
		//�����ӵ�������
		//

		const float AmmoPerIcon = Weapon->GetAmmoPerClip() / AmmoIconCount;
		//�ֱ����ÿһ������
		for (int32 i = 0; i < AmmoIconCount; i++)
		{
			
			if ((i + 1)*AmmoPerIcon > Weapon->GetCurrentAmmoInClip())
			{
				const float UsedPercentAmmo = AmmoPerIcon * (i + 1) - Weapon->GetCurrentAmmoInClip();
				float PercentLeftInICon = 0;
				if(UsedPercentAmmo<AmmoPerIcon)
				{
					PercentLeftInICon = (AmmoPerIcon - UsedPercentAmmo)/AmmoPerIcon;
				}
				const int32 Color = 128 + 128 * PercentLeftInICon;
				Canvas->SetDrawColor(Color, Color, Color, Color);
			}
			const float ClipOffset = PriClipIconOffset * i * ScaleUI;
			Canvas->DrawIcon(PrimaryClipIcon, PriClipPosX + ClipOffset, PriClipPosY, ScaleUI);
		}
	}
	

}