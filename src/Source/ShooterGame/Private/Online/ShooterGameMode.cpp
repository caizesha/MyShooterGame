// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterGameMode.h"
#include "ShooterCharacter.h"
#include "ShooterPlayerController.h"
#include "ShooterPlayerState.h"
#include "ShooterGameState.h"
#include "ShooterSpectatorPawn.h"
#include "ShooterHUD.h"

AShooterGameMode::AShooterGameMode()
{
	//ģ�庯����ͨ�����Ҹ����������Ҷ�Ӧ����ͼ��playerpawn����������Ӧ�Ķ��󣨸���ΪASHooterCharacter������ָ�����ࣩ
	static ConstructorHelpers::FClassFinder<AShooterCharacter> PlayerPawnOb(TEXT("/Game/Blueprints/Pawns/PlayerPawn"));
	//��ֵplayerpawn�ķ�����Ϣ�����
	DefaultPawnClass = PlayerPawnOb.Class;
	//������ҿ�������ķ�����Ϣ
	PlayerControllerClass = AShooterPlayerController::StaticClass();
	//�������״̬��ķ�����Ϣ
	PlayerStateClass = AShooterPlayerState::StaticClass();
	//������Ϸ״̬��ķ�����Ϣ
	GameStateClass = AShooterGameState::StaticClass();
	//�����۲�����ķ�����Ϣ
	SpectatorClass = AShooterSpectatorPawn::StaticClass();
	//�������������ķ�����Ϣ
	HUDClass = AShooterHUD::StaticClass();
}


