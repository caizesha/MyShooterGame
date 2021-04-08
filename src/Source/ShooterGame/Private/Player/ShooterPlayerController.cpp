// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterPlayerController.h"
#include "ShooterPlayerCameraManager.h"
#include "Online.h"
#include "OnlineSessionInterface.h"
#include "ShooterGameInstance.h"

AShooterPlayerController::AShooterPlayerController()
{
	PlayerCameraManagerClass = AShooterPlayerCameraManager::StaticClass();//����Camera(StaticClass�������������䣩
}

void AShooterPlayerController::ClientStartOnlineGame_Implementation()
{
	//ֻ��������Ҽ��
	if (!IsPrimaryPlayer())
	{
		return;
	}
	//������ڣ�˵���Ѿ�ͬ��
	if (PlayerState)
	{
		IOnlineSubsystem* OnlineSubsys = IOnlineSubsystem::Get();
		if (OnlineSubsys)
		{
			IOnlineSessionPtr Session = OnlineSubsys->GetSessionInterface();
			if (Session.IsValid())
			{
				Session->StartSession(PlayerState->SessionName);
			}
		}
	}

	else
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ClientStartOnlineGame, this, &AShooterPlayerController::ClientStartOnlineGame_Implementation, 0.1f, false);
	}
	
}

void AShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("BackToGameMenu", IE_Pressed, this, &AShooterPlayerController::OnToggleInGameMenu);
}

void AShooterPlayerController::OnToggleInGameMenu()
{
	if (GEngine->GameViewport == nullptr)
	{
		return;
	}
	UShooterGameInstance* const GI = Cast<UShooterGameInstance>(GetGameInstance());
	return GI ? GI->GotoInitialState() : nullptr;
}