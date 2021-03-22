// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterGameMode.h"
#include "ShooterCharacter.h"
#include "ShooterPlayerController.h"
#include "ShooterPlayerState.h"
#include "ShooterGameState.h"
#include "ShooterSpectatorPawn.h"
#include "ShooterHUD.h"
#include "ShooterAIController.h"

AShooterGameMode::AShooterGameMode()
{
	//模板函数，通过查找辅助类来查找对应的蓝图类playerpawn，创建出对应的对象（父类为ASHooterCharacter，父类指向子类）
	static ConstructorHelpers::FClassFinder<AShooterCharacter> PlayerPawnOb(TEXT("/Game/Blueprints/Pawns/PlayerPawn"));
	//赋值playerpawn的反射信息类对象
	DefaultPawnClass = PlayerPawnOb.Class;
	
	//赋值botpawn的反射信息类对象
	static ConstructorHelpers::FClassFinder<AShooterCharacter> BotPawnOb(TEXT("/Game/Blueprints/Pawns/BotPawn"));
	BotPawnClass = BotPawnOb.Class;

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

void AShooterGameMode::StartPlay()
{
	Super::StartPlay();

	CreateBotController();

	AddBots();
}

AShooterAIController* AShooterGameMode::CreateBotController()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = nullptr;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.OverrideLevel = nullptr;

	AShooterAIController* AIController = GetWorld()->SpawnActor<AShooterAIController>(SpawnInfo);
	return AIController;
}

void AShooterGameMode::AddBots()
{
	UWorld* world = GetWorld();
	for (FConstControllerIterator It = world->GetControllerIterator(); It; It++)
	{
		AShooterAIController* AIC = Cast<AShooterAIController>(*It);
		if(AIC)
		{
			RestartPlayer(AIC);
		}
	}
}

UClass* AShooterGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (InController->IsA<AShooterAIController>())
	{
		return BotPawnClass;
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AShooterGameMode::DeafultTimer()
{
	//实现倒数计时器
	AShooterGameState* ShooterGameState = Cast<AShooterGameState>(GameState);
	if (ShooterGameState && (ShooterGameState->RemainingTime) > 0)
	{
		ShooterGameState->RemainingTime--;
		if (ShooterGameState->RemainingTime <= 0)
		{
			RestartGame();
		}
	}
	
}

void AShooterGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	//设置倒数定时器
	GetWorldTimerManager().SetTimer(TimeHandle_DefaultTimer, this, &AShooterGameMode::DeafultTimer, 1.0f, true);
}

void AShooterGameMode::HandleMatchHasStarted()
{
	//配置游戏剩余时间
	Super::HandleMatchHasStarted();
	AShooterGameState* ShooterGameState = Cast<AShooterGameState>(GameState);
	ShooterGameState->RemainingTime = RoundTime;
}
