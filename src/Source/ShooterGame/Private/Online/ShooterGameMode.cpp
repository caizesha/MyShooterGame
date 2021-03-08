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
	//模板函数，通过查找辅助类来查找对应的蓝图类playerpawn，创建出对应的对象（父类为ASHooterCharacter，父类指向子类）
	static ConstructorHelpers::FClassFinder<AShooterCharacter> PlayerPawnOb(TEXT("/Game/Blueprints/Pawns/PlayerPawn"));
	//赋值playerpawn的反射信息类对象
	DefaultPawnClass = PlayerPawnOb.Class;
	//创建玩家控制器类的反射信息
	PlayerControllerClass = AShooterPlayerController::StaticClass();
	//创建玩家状态类的反射信息
	PlayerStateClass = AShooterPlayerState::StaticClass();
	//创建游戏状态类的反射信息
	GameStateClass = AShooterGameState::StaticClass();
	//创建观察者类的反射信息
	SpectatorClass = AShooterSpectatorPawn::StaticClass();
	//创建交显面板类的反射信息
	HUDClass = AShooterHUD::StaticClass();
}


